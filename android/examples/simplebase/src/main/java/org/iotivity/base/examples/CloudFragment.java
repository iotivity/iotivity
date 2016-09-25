/*
 * ******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Fragment;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.base.AclGroupType;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPresenceHandle;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;

/**
 * This class is for messaging between the cloud server and the client.
 * It can handle cloud API manually.
 */
public class CloudFragment extends Fragment implements
        View.OnClickListener, CompoundButton.OnCheckedChangeListener,
        OcResource.OnObserveListener,
        OcResource.OnMQTopicFoundListener, OcResource.OnMQTopicCreatedListener,
        OcResource.OnMQTopicSubscribeListener {

    private static final String TAG = "OIC_SIMPLE_CLOUD";
    private final String EOL = System.getProperties().getProperty("line.separator");
    private final Pattern ADDRESS_PORT
            = Pattern.compile(
            "((25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9])" +
                    "\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)" +
                    "\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[1-9]|0)" +
                    "\\.(25[0-5]|2[0-4][0-9]|[0-1][0-9]{2}|[1-9][0-9]|[0-9])" +
                    ":([0-9]{1,5}))");

    private Activity mActivity;
    private Context mContext;

    private QualityOfService mQos = QualityOfService.LOW;
    private boolean mSecured = false;

    private LinearLayout mAccountLayout;
    private LinearLayout mRDLayout;
    private LinearLayout mMQLayout;

    private TextView mAccountText;
    private TextView mRDText;
    private TextView mMQText;
    private ScrollView mScrollView;
    private TextView mActionLog;
    private TextView mResultLog;

    private OcAccountManager mAccountManager;
    private String mAccessToken;
    private String mRefreshToken;
    private String mUserUuid;
    private String mGroupId;
    private String mGroupMasterId;
    private String mInviterUserId;
    private String mInviteeUuid;
    private final int REQUEST_LOGIN = 1;

    private OcResourceHandle localLightResourceHandle = null;
    private List<OcResourceHandle> mResourceHandleList = new LinkedList<>();
    private OcPresenceHandle mOcPresenceHandle = null;
    private Button mDevicePresenceButton;

    private OcResource MQbrokerResource = null;
    private OcResource currentTopicResource = null;
    private boolean switchingFlag = true;
    private int roomNum = 1;
    private String defaultTopicFullName = Common.MQ_DEFAULT_TOPIC_URI;

    // variables related observer
    private int maxSequenceNumber = 0xFFFFFF;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mActivity = getActivity();
        mContext = mActivity.getBaseContext();
        initOcPlatform(ModeType.CLIENT_SERVER);
    }

    private void initOcPlatform(ModeType type) {
        PlatformConfig cfg = new PlatformConfig(mActivity, mContext,
                ServiceType.IN_PROC,
                type,
                Common.IP_ADDRESS,
                Common.IP_PORT,
                mQos);
        OcPlatform.Configure(cfg);
    }

    private void signUp() {
        try {
            mAccountManager = OcPlatform.constructAccountManagerObject(
                    Common.HOST,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));
        } catch (OcException e) {
            e.printStackTrace();
        }

        Intent intentLogin = new Intent(mContext, LoginActivity.class);
        startActivityForResult(intentLogin, REQUEST_LOGIN);
    }

    private void signIn() {
        try {
            msg("signIn");
            mAccountManager.signIn(mUserUuid, mAccessToken, onSignIn);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void signOut() {
        try {
            msg("signOut");
            mAccountManager.signOut(onSignOut);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void createGroup() {
        try {
            msg("createGroup");
            mAccountManager.createGroup(AclGroupType.PUBLIC, onCreateGroup);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void inviteUser() {
        if (mGroupId == null) {
            msg("there is no any group");
        } else {
            showInviteUser();
        }
    }

    private void leaveGroup() {
        try {
            if (mGroupId == null) {
                msg("there is no any group");
            } else {
                msg("leaveGroup");
                mAccountManager.leaveGroup(mGroupId, onLeaveGroup);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    // FOR WEB-VIEW
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == Activity.RESULT_OK && requestCode == REQUEST_LOGIN) {
            String authCode = data.getStringExtra("authCode");
            msg("\tauthCode: " + authCode);

            try {
                msg("Sign Up");
                mAccountManager.signUp("github", authCode, onSignUp);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    OcAccountManager.OnPostListener onSignUp = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            msg("signUp was successful");
            try {
                mUserUuid = ocRepresentation.getValue("uid");
                mAccessToken = ocRepresentation.getValue("accesstoken");
                mRefreshToken = ocRepresentation.getValue("refreshtoken");
                String tokenType = ocRepresentation.getValue("tokentype");
                msg("\tuserID: " + mUserUuid);
                msg("\taccessToken: " + mAccessToken);
                msg("\trefreshToken: " + mRefreshToken);
                msg("\ttokenType: " + tokenType);

                if (ocRepresentation.hasAttribute("expiresin")) {
                    int expiresIn = ocRepresentation.getValue("expiresin");
                    msg("\texpiresIn: " + expiresIn);
                }
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            msg("Failed to signUp");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnPostListener onSignIn = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            msg("signIn was successful");
            try {
                msg("observeInvitation");
                mAccountManager.observeInvitation(onObserveInvitation);
                msg("getGroupList");
                mAccountManager.getGroupList(onGetGroupList);
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            msg("Failed to signIn");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnPostListener onSignOut = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            msg("signOut was successful");
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            msg("Failed to signOut");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnGetListener onGetGroupList = new OcAccountManager.OnGetListener() {
        @Override
        public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
            msg("getGroupList was successful");
            try {
                String[] gidlist = ocRepresentation.getValue("gidlist");
                if (gidlist == null) {
                    msg("\tgroup list is empty");
                    mGroupId = null;
                } else {
                    msg("\tgroup list");
                    for (String group : gidlist) {
                        msg("\t\t" + group);
                        mGroupId = group;
                    }
                    msg("\tcurrent group is " + mGroupId);
                }
            } catch (OcException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onGetFailed(Throwable throwable) {
            msg("Failed to getGroupList");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnObserveListener onObserveInvitation =
            new OcAccountManager.OnObserveListener() {
                @Override
                public void onObserveCompleted(List<OcHeaderOption> list,
                                               OcRepresentation ocRepresentation, int i) {
                    msg("observeInvitation was successful");
                    try {
                        if (REGISTER == i) {
                            msg("REGISTER was successful");

                            OcRepresentation[] sendInvitationList =
                                    ocRepresentation.getValue("invite");
                            if (sendInvitationList != null) {
                                msg("\tList of invitation that I sent");
                                for (OcRepresentation invitation : sendInvitationList) {
                                    String gid = invitation.getValue("gid");
                                    String mid = invitation.getValue("mid");
                                    msg("\t\t-GroupID : " + gid);
                                    msg("\t\t InviteeID : " + mid);
                                }
                            }

                            OcRepresentation[] receiveInvitationList =
                                    ocRepresentation.getValue("invited");
                            if (receiveInvitationList != null) {
                                msg("\tList of invitation that I received");
                                for (OcRepresentation invitation : receiveInvitationList) {
                                    String gid = invitation.getValue("gid");
                                    String mid = invitation.getValue("mid");
                                    msg("\t\t-GroupID : " + gid);
                                    msg("\t\t InviterID : " + mid);
                                }
                            }
                        } else if (DEREGISTER == i) {
                            msg("DEREGISTER was successful");
                        } else {
                            OcRepresentation[] sendInvitationList =
                                    ocRepresentation.getValue("invite");
                            if (sendInvitationList != null) {
                                msg("\tList of invitation that I sent");
                                for (OcRepresentation invitation : sendInvitationList) {
                                    String gid = invitation.getValue("gid");
                                    String mid = invitation.getValue("mid");
                                    msg("\t\t-GroupID : " + gid);
                                    msg("\t\t InviteeID : " + mid);
                                }
                            }

                            OcRepresentation[] receivInvitationList =
                                    ocRepresentation.getValue("invited");
                            if (receivInvitationList != null) {
                                msg("\tList of invitation that I received");
                                for (OcRepresentation invitation : receivInvitationList) {
                                    mGroupId = invitation.getValue("gid");
                                    mGroupMasterId = invitation.getValue("mid");
                                    msg("\t\t-GroupID : " + mGroupId);
                                    msg("\t\t InviterID : " + mGroupMasterId);
                                }
                                msg("searchUser");
                                mAccountManager.searchUser(mGroupMasterId, onSearchUserForInvitee);
                            }
                        }
                    } catch (OcException e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onObserveFailed(Throwable throwable) {
                    msg("Failed to observeInvitation");
                    if (throwable instanceof OcException) {
                        OcException ocEx = (OcException) throwable;
                        Log.e(TAG, ocEx.toString());
                        ErrorCode errCode = ocEx.getErrorCode();
                        msg("Error code: " + errCode);
                    }
                }
            };

    OcAccountManager.OnGetListener onSearchUserForInvitee = new OcAccountManager.OnGetListener() {
        @Override
        public synchronized void onGetCompleted(List<OcHeaderOption> list,
                                                OcRepresentation ocRepresentation) {
            msg("searchUser was successful");
            try {
                OcRepresentation[] userList = ocRepresentation.getValue("ulist");
                for (OcRepresentation user : userList) {
                    String inviterUuid = user.getValue("uid");
                    Log.d(TAG, "inviterUuid : " + inviterUuid);

                    OcRepresentation userInfo = user.getValue("uinfo");
                    mInviterUserId = userInfo.getValue("userid");
                }

                mActivity.runOnUiThread(new Runnable() {
                    public void run() {
                        showInviteMsg(mInviterUserId);
                    }
                });

            } catch (OcException e) {
                e.printStackTrace();
            }
        }

        @Override
        public synchronized void onGetFailed(Throwable throwable) {
            msg("Failed to searchUser");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnPostListener onJoinGroup = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            msg("joinGroup was successful");
            try {
                msg("observeGroup");
                mAccountManager.observeGroup(mGroupId, onObserveGroup);
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            msg("Failed to joinGroup");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnDeleteListener onDeleteInvitation = new OcAccountManager.OnDeleteListener() {
        @Override
        public void onDeleteCompleted(List<OcHeaderOption> list) {
            msg("deleteInvitation was successful");
            try {
                mAccountManager.getGroupList(onGetGroupList);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onDeleteFailed(Throwable throwable) {
            msg("Failed to deleteInvitation");
        }
    };

    OcAccountManager.OnGetListener onSearchUserForInviter = new OcAccountManager.OnGetListener() {
        @Override
        public synchronized void onGetCompleted(List<OcHeaderOption> list,
                                                OcRepresentation ocRepresentation) {
            msg("searchUser was successful");
            try {
                OcRepresentation[] userList = ocRepresentation.getValue("ulist");
                for (OcRepresentation user : userList) {
                    mInviteeUuid = user.getValue("uid");
                    OcRepresentation userInfo = user.getValue("uinfo");
                    String inviteeUserId = userInfo.getValue("userid");
                    Log.d(TAG, "inviteeUserId : " + inviteeUserId);
                }
                msg("sendInvitation");
                mAccountManager.sendInvitation(mGroupId, mInviteeUuid, onSendInvitation);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }

        @Override
        public synchronized void onGetFailed(Throwable throwable) {
            msg("Failed to searchUser");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnPostListener onCreateGroup = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            msg("createGroup was successful");
            try {
                mGroupId = ocRepresentation.getValue("gid");
                msg("\tgroupId: " + mGroupId);

                msg("observeGroup");
                mAccountManager.observeGroup(mGroupId, onObserveGroup);
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            msg("Failed to createGroup");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnObserveListener onObserveGroup = new OcAccountManager.OnObserveListener() {
        @Override
        public void onObserveCompleted(List<OcHeaderOption> list,
                                       OcRepresentation ocRepresentation, int i) {
            msg("observeGroup was successful");
            try {
                if (REGISTER == i) {
                    msg("REGISTER was successful");
                } else if (DEREGISTER == i) {
                    msg("DEREGISTER was successful");
                } else {
                    String gid = ocRepresentation.getValue("gid");
                    msg("\tGroupID: " + gid);

                    String gmid = ocRepresentation.getValue("gmid");
                    msg("\tGroupMasterID: " + gmid);

                    String[] midlist = ocRepresentation.getValue("midlist");
                    if (midlist == null) {
                        msg("\tMember List is empty");
                    } else {
                        msg("\tMember List(" + midlist.length + ")");
                        for (String mid : midlist) {
                            msg("\t : " + mid);
                        }
                    }

                    String[] dilist = ocRepresentation.getValue("dilist");
                    if (dilist == null) {
                        msg("\tDevice List is empty");
                    } else {
                        msg("\tDevice List(" + dilist.length + ")");
                        for (String di : dilist) {
                            msg("\t : " + di);
                        }
                    }
                }
            } catch (OcException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onObserveFailed(Throwable throwable) {
            msg("Failed to observeGroup");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnPostListener onSendInvitation = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            msg("sendInvitation was successful");
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            msg("Failed to sendInvitation");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                msg("Error code: " + errCode);
            }
        }
    };

    OcAccountManager.OnDeleteListener onLeaveGroup = new OcAccountManager.OnDeleteListener() {
        @Override
        public void onDeleteCompleted(List<OcHeaderOption> list) {
            msg("leaveGroup was successful");
            try {
                msg("getGroupList");
                mAccountManager.getGroupList(onGetGroupList);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onDeleteFailed(Throwable throwable) {
            msg("Failed to leaveGroup");
        }
    };

    // ******************************************************************************
    // End of the Account Manager specific code
    // ******************************************************************************

    OcPlatform.OnPublishResourceListener resourcePublishListener =
            new OcPlatform.OnPublishResourceListener() {
                @Override
                public void onPublishResourceCompleted(OcRepresentation ocRepresentation) {
                    msg("onPublishResourceCompleted");

                    for (OcRepresentation child : ocRepresentation.getChildren()) {
                        try {
                            msg("\tPublished Resource URI : " + child.getValue("href"));
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    }
                }

                @Override
                public void onPublishResourceFailed(Throwable throwable) {
                    msg("onPublishResourceFailed has failed");
                }
            };

    OcPlatform.OnDeleteResourceListener resourceDeleteListener =
            new OcPlatform.OnDeleteResourceListener() {
                @Override
                public void onDeleteResourceCompleted(int resultCode) {
                    msg("onDeleteResourceCompleted, result is " + resultCode);
                }
            };

    private void createResource() {
        if (localLightResourceHandle == null) {
            try {
                localLightResourceHandle = OcPlatform.registerResource(
                        Common.RESOURCE_URI,            //resource URI
                        Common.RESOURCE_TYPE,           //resource type name
                        Common.RESOURCE_INTERFACE,      //using default interface
                        null,                           //use default entity handler
                        EnumSet.of(ResourceProperty.DISCOVERABLE)
                );
                mResourceHandleList.add(localLightResourceHandle);
                msg("Create Local Resource is success.");
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }
    }

    private void publishResourceToRD() {
        // Create Local Resource.
        createResource();

        try {
            // Publish Virtual Resource to Resource-Directory.
            Log.d(TAG, "Publish Virtual Resource to Resource-Directory.");
            OcPlatform.publishResourceToRD(
                    Common.HOST, EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    resourcePublishListener
            );

            // Publish Local Resource to Resource-Directory.
            Log.d(TAG, "Publish Local Resource to Resource-Directory.");
            OcPlatform.publishResourceToRD(
                    Common.HOST, EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP), mResourceHandleList,
                    resourcePublishListener
            );
        } catch (OcException e) {
            Log.e(TAG, e.toString());
        }
    }

    private void deleteResourceFromRD() {
        try {
            // Delete Resource from Resource-Directory.
            Log.d(TAG, "Delete Resource from Resource-Directory.");
            OcPlatform.deleteResourceFromRD(
                    Common.HOST, EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    resourceDeleteListener
            );
        } catch (OcException e) {
            Log.e(TAG, e.toString());
        }
    }

    private void subscribeDevicePresence() {
        try {
            if (null == mOcPresenceHandle) {
                List<String> di = new ArrayList<>();
                mOcPresenceHandle = OcPlatform.subscribeDevicePresence(
                        Common.HOST, di, EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP), this);
                mDevicePresenceButton.setText(R.string.unsub_presence);
            } else {
                OcPlatform.unsubscribePresence(mOcPresenceHandle);
                mOcPresenceHandle = null;
                mDevicePresenceButton.setText(R.string.sub_presence);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> list,
                                   OcRepresentation ocRepresentation, int sequenceNumber) {
        if (sequenceNumber != maxSequenceNumber + 1) {
            msg("OBSERVE Result:");
            msg("\tSequenceNumber:" + sequenceNumber);
            try {
                if (ocRepresentation.hasAttribute("prslist")) {
                    OcRepresentation[] prslist = ocRepresentation.getValue("prslist");
                    if (prslist != null) {
                        msg("\tDevice Presence");
                        for (OcRepresentation prs : prslist) {
                            msg("\t\tDevice ID : " + prs.getValue("di"));
                            msg("\t\tState : " + prs.getValue("state"));
                        }
                    }
                }
            } catch (OcException e) {
                e.printStackTrace();
            }
        } else {
            msg("Successful unsubscribePresence");
        }
    }

    @Override
    public void onObserveFailed(Throwable throwable) {

    }

    // ******************************************************************************
    // End of the Resource Directory specific code
    // ******************************************************************************

    void getMQBroker() {
        List<String> resourceTypeList = new ArrayList<>();
        List<String> resourceInterfaceList = new ArrayList<>();
        resourceInterfaceList.add(Common.RESOURCE_INTERFACE);
        resourceTypeList.add("ocf.wk.ps");
        try {
            MQbrokerResource = OcPlatform.constructResourceObject(
                    Common.HOST,
                    Common.MQ_BROKER_URI,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP, OcConnectivityType.CT_IP_USE_V4),
                    false,
                    resourceTypeList, resourceInterfaceList);

            msg("found MQ broker : " + MQbrokerResource.getHost());

            discoveryMQTopics();

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    void discoveryMQTopics() {
        try {
            if (null != MQbrokerResource) {
                MQbrokerResource.discoveryMQTopics(
                        new HashMap<String, String>(),
                        this, QualityOfService.LOW);
            }

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    synchronized public void onTopicDiscoveried(OcResource ocResource) {
        synchronized (this) {
            String resourceUri = ocResource.getUri();

            msg("onTopicDiscoveried : " + resourceUri + " found");
        }
    }

    @Override
    public void onDiscoveryTopicFailed(Throwable ex, String uri) {
        Log.e(TAG, "onFindTopicFailed : ", ex);

        if (ex instanceof OcException) {
            OcException ocEx = (OcException) ex;
            ErrorCode errCode = ocEx.getErrorCode();
            Log.d(TAG, "onFindTopicFailed Code: " + errCode);
            Log.d(TAG, "onFindTopicFailed Code: " + errCode.ordinal());
            Log.d(TAG, "onFindTopicFailed uri: " + uri);

        } else {
            Log.e(TAG, ex.getMessage());
        }
    }

    void createMQTopic() {
        try {
            if (null != MQbrokerResource) {
                Map<String, String> queryParameters = new HashMap<>();
                queryParameters.put("rt", "light");
                MQbrokerResource.createMQTopic(
                        new OcRepresentation(),
                        defaultTopicFullName,
                        queryParameters,
                        this,
                        QualityOfService.LOW);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    synchronized public void onTopicResourceCreated(OcResource ocResource) {
        synchronized (this) {
            Log.d(TAG, "onTopicResourceCreated");
            currentTopicResource = ocResource;

            msg("onTopicResourceCreated : " + currentTopicResource.getUri());
        }
    }

    @Override
    public void onCreateTopicFailed(Throwable ex, String uri) {
        Log.e(TAG, "onCreateTopicFailed : ", ex);

        if (ex instanceof OcException) {
            OcException ocEx = (OcException) ex;
            ErrorCode errCode = ocEx.getErrorCode();
            Log.d(TAG, "onCreateTopicFailed error Code: " + errCode);
            Log.d(TAG, "onCreateTopicFailed error Code: " + errCode.ordinal());
            Log.d(TAG, "onCreateTopicFailed error uri: " + uri);

            // retry to create after increase room number
            defaultTopicFullName = Common.MQ_DEFAULT_TOPIC_URI + (roomNum++);
            createMQTopic();
        } else {
            Log.e(TAG, ex.getMessage());
        }
    }

    void subscribeMQTopic() {
        Map<String, String> queryParameters = new HashMap<>();
        queryParameters.put("rt", "light");

        try {
            if (null != currentTopicResource) {
                currentTopicResource.subscribeMQTopic(
                        queryParameters,
                        this,
                        QualityOfService.LOW);
            }

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    synchronized public void onSubScribeCompleted(List<OcHeaderOption> headerOptionList,
                                                  OcRepresentation ocRepresentation,
                                                  int sequenceNumber) {
        synchronized (this) {
            String resourceUri = ocRepresentation.getUri();
            Log.d(TAG, "onSubScribeCompleted sequenceNumber : " + sequenceNumber);

            try {
                int subFlag;
                OcRepresentation val = ocRepresentation.getValue("message");

                if (sequenceNumber == 0) {
                    Log.d(TAG, "onSubScribeCompleted : " + resourceUri);
                    subFlag = 0;
                } else {
                    subFlag = 1;
                }

                if (subFlag == 0) {
                    msg("onSubScribeCompleted : " + resourceUri);
                } else {
                    Log.d(TAG, "onSubScribeCompleted : " + resourceUri);
                    Log.d(TAG, "onSubScribeCompleted : " + val.getValue("blue"));
                    Log.d(TAG, "onSubScribeCompleted : " + val.getValue("red"));

                    msg("onSubScribeCompleted : " + resourceUri + ", blue light is "
                            + val.getValue("blue").toString());
                    msg("onSubScribeCompleted : " + resourceUri + ", red light is "
                            + val.getValue("red").toString());
                }
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    synchronized public void onUnsubScribeCompleted(OcRepresentation ocRepresentation,
                                                    int sequenceNumber) {
        synchronized (this) {
            String resourceUri = ocRepresentation.getUri();
            Log.d(TAG, "onUnsubScribeCompleted sequenceNumber : " + sequenceNumber);

            if (sequenceNumber == maxSequenceNumber + 1) {
                Log.d(TAG, "onUnsubScribeCompleted : " + resourceUri);
                msg("onUnsubScribeCompleted : " + resourceUri);
            }
        }
    }

    @Override
    public void onSubScribeFailed(Throwable ex) {
        Log.d(TAG, "onSubScribeFailed : ", ex);

        if (ex instanceof OcException) {
            OcException ocEx = (OcException) ex;
            ErrorCode errCode = ocEx.getErrorCode();
            Log.d(TAG, "onSubScribeFailed error Code: " + errCode);
            Log.d(TAG, "onSubScribeFailed error Code: " + errCode.ordinal());


        } else {
            Log.e(TAG, ex.getMessage());
        }
    }


    void unsubscribeMQTopic() {

        try {
            if (null != currentTopicResource) {
                currentTopicResource.unsubscribeMQTopic(QualityOfService.LOW);
            }

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    void publishMQTopic() {

        try {
            OcRepresentation rep = new OcRepresentation();
            OcRepresentation msg = new OcRepresentation();

            if (switchingFlag) {
                msg.setValue("blue", "on");
                msg.setValue("red", "off");
                switchingFlag = false;
            } else {
                msg.setValue("blue", "off");
                msg.setValue("red", "on");
                switchingFlag = true;
            }
            rep.setValue("message", msg);

            if (null != currentTopicResource) {
                currentTopicResource.publishMQTopic(rep,
                        new HashMap<String, String>(),
                        mqPublishListener,
                        QualityOfService.LOW);
            }

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    OcResource.OnPostListener mqPublishListener =
            new OcResource.OnPostListener() {
                @Override
                public void onPostCompleted(List<OcHeaderOption> list,
                                            OcRepresentation ocRepresentation) {
                    Log.i(TAG, "onPublish completed");
                    msg("onPublish completed");
                }

                @Override
                public void onPostFailed(Throwable throwable) {
                    Log.e(TAG, "onPublish failed");
                    msg("onPublish failed");
                }
            };

    void requestMQPublish() {

        try {
            if (null != currentTopicResource) {
                currentTopicResource.requestMQPublish(
                        new HashMap<String, String>(),
                        mqReqPubListener,
                        QualityOfService.LOW);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    OcResource.OnPostListener mqReqPubListener =
            new OcResource.OnPostListener() {
                @Override
                public void onPostCompleted(List<OcHeaderOption> list,
                                            OcRepresentation ocRepresentation) {
                    Log.i(TAG, "onRequestPublish completed");
                    msg("onPublish completed");
                }

                @Override
                public void onPostFailed(Throwable throwable) {
                    Log.e(TAG, "onRequestPublish failed");
                    msg("onRequestPublish failed");
                }
            };

    // ******************************************************************************
    // End of the Message Queue specific code
    // ******************************************************************************

    private void showTCPInput() {

        LayoutInflater layoutInflater = LayoutInflater.from(mContext);
        View inputView = layoutInflater.inflate(R.layout.input, null);
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(mActivity);
        alertDialogBuilder.setView(inputView);

        final EditText editText = (EditText) inputView.getRootView().findViewById(R.id.inputText);
        final CheckBox isSecured = (CheckBox) inputView.getRootView().findViewById(R.id.secured);

        StringBuilder sb = new StringBuilder();
        sb.append(Common.TCP_ADDRESS);
        sb.append(Common.PORT_SEPARATOR);
        sb.append(Common.TCP_PORT);
        editText.setText(sb.toString());

        isSecured.setVisibility(View.VISIBLE);
        isSecured.setChecked(mSecured);

        alertDialogBuilder
                .setCancelable(true)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        if (editText.getText().length() != 0) {
                            final String hosts = editText.getText().toString();
                            boolean isValid = ADDRESS_PORT.matcher(hosts).matches();
                            if (isValid) {
                                final String host[] = hosts.split(Common.PORT_SEPARATOR);
                                Common.TCP_ADDRESS = host[0];
                                Common.TCP_PORT = host[1];
                                mSecured = isSecured.isChecked();

                                StringBuilder sb = new StringBuilder();
                                if (mSecured) {
                                    sb.append(Common.COAPS_TCP);
                                } else {
                                    sb.append(Common.COAP_TCP);
                                }
                                sb.append(Common.TCP_ADDRESS);
                                sb.append(Common.PORT_SEPARATOR);
                                sb.append(Common.TCP_PORT);
                                Common.HOST = sb.toString();
                            } else {
                                Toast.makeText(mContext, "Invalid IP", Toast.LENGTH_SHORT).show();
                                showTCPInput();
                            }
                        }
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        AlertDialog alert = alertDialogBuilder.create();
        alert.show();
    }

    private void showInviteUser() {

        LayoutInflater layoutInflater = LayoutInflater.from(mContext);
        View inputView = layoutInflater.inflate(R.layout.input, null);
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(mActivity);
        alertDialogBuilder.setView(inputView);

        final TextView textView = (TextView) inputView.getRootView().findViewById(R.id.inputView);
        textView.setText("Please enter user ID to invite.");
        final EditText editText = (EditText) inputView.getRootView().findViewById(R.id.inputText);

        alertDialogBuilder
                .setCancelable(true)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        if (editText.getText().length() != 0) {
                            final String userID = editText.getText().toString();

                            try {
                                Map<String, String> option = new HashMap<>();
                                option.put("userid", userID);

                                msg("searchUser (User ID: " + userID + ")");
                                mAccountManager.searchUser(option, onSearchUserForInviter);
                            } catch (OcException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        AlertDialog alert = alertDialogBuilder.create();
        alert.show();
    }

    private void showInviteMsg(String userID) {

        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(mActivity);
        alertDialogBuilder.setTitle("Invitation");
        StringBuilder sb = new StringBuilder();
        sb.append("Invited from ");
        sb.append(userID);
        sb.append(EOL);
        sb.append("Accept?");
        alertDialogBuilder.setMessage(sb.toString());
        alertDialogBuilder
                .setCancelable(true)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        try {
                            msg("joinGroup");
                            mAccountManager.joinGroup(mGroupId, onJoinGroup);
                            msg("deleteInvitation");
                            mAccountManager.deleteInvitation(mGroupId, onDeleteInvitation);
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                        try {
                            msg("gid: " + mGroupId);
                            msg("deleteInvitation");
                            mAccountManager.deleteInvitation(mGroupId, onDeleteInvitation);
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    }
                });

        AlertDialog alert = alertDialogBuilder.create();
        alert.show();
    }

    private void msg(final String text) {
        mActivity.runOnUiThread(new Runnable() {
            public void run() {
                mResultLog.append(EOL);
                mResultLog.append(text);
                mScrollView.fullScroll(View.FOCUS_DOWN);
            }
        });
        Log.i(TAG, text);
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

        int visible;
        if (isChecked) {
            visible = View.VISIBLE;
        } else {
            visible = View.GONE;
        }

        switch (buttonView.getId()) {
            case R.id.account_switch:
                mAccountLayout.setVisibility(visible);
                break;
            case R.id.rd_switch:
                mRDLayout.setVisibility(visible);
                break;
            case R.id.mq_switch:
                mMQLayout.setVisibility(visible);
                break;
        }
    }

    @Override
    public void onClick(View view) {
        mActionLog.setText("[Action Log]" + EOL);

        int viewID = view.getId();
        if (mAccountManager == null && (viewID == R.id.signin_button
                || viewID == R.id.signout_button || viewID == R.id.creategroup_button
                || viewID == R.id.invite_button || viewID == R.id.leavegroup_button)) {
            mActionLog.append("Do 'SignUp' first" + EOL);
            return;
        }

        switch (viewID) {
            // Account
            case R.id.setip_button:
                mActionLog.append("Set IP" + EOL);
                showTCPInput();
                break;
            case R.id.signup_button:
                mActionLog.append("Sign Up" + EOL);
                signUp();
                break;
            case R.id.signin_button:
                mActionLog.append("Sign In" + EOL);
                signIn();
                break;
            case R.id.signout_button:
                mActionLog.append("Sign Out" + EOL);
                signOut();
                break;
            case R.id.creategroup_button:
                mActionLog.append("Create Group" + EOL);
                createGroup();
                break;
            case R.id.invite_button:
                mActionLog.append("Invite User" + EOL);
                inviteUser();
                break;
            case R.id.leavegroup_button:
                mActionLog.append("Leave Group" + EOL);
                leaveGroup();
                break;

            // RD
            case R.id.rdpub_button:
                mActionLog.append("Publish Resource To RD" + EOL);
                publishResourceToRD();
                break;
            case R.id.rddel_button:
                mActionLog.append("Delete Resource From RD" + EOL);
                deleteResourceFromRD();
                break;
            case R.id.rddp_button:
                mActionLog.append("Subscribe Device Presence" + EOL);
                subscribeDevicePresence();
                break;

            // MQ
            case R.id.mqget_button:
                mActionLog.append("Get MQ Broker" + EOL);
                getMQBroker();
                break;
            case R.id.mqcreate_button:
                mActionLog.append("Create MQ Topic" + EOL);
                createMQTopic();
                break;
            case R.id.mqsub_button:
                mActionLog.append("Subscribe MQ Topic" + EOL);
                subscribeMQTopic();
                break;
            case R.id.mqunsub_button:
                mActionLog.append("Un-subscribe MQ Topic" + EOL);
                unsubscribeMQTopic();
                break;
            case R.id.mqpub_button:
                mActionLog.append("Publish MQ Topic" + EOL);
                publishMQTopic();
                break;
            case R.id.mqreq_button:
                mActionLog.append("Request MQ Publish" + EOL);
                requestMQPublish();
                break;
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_cloud, container, false);

        mAccountLayout = (LinearLayout) rootView.findViewById(R.id.account_layout);
        mRDLayout = (LinearLayout) rootView.findViewById(R.id.rd_layout);
        mMQLayout = (LinearLayout) rootView.findViewById(R.id.mq_layout);

        Switch accountSwitch = (Switch) rootView.findViewById(R.id.account_switch);
        Switch RDSwitch = (Switch) rootView.findViewById(R.id.rd_switch);
        Switch MQSwitch = (Switch) rootView.findViewById(R.id.mq_switch);
        accountSwitch.setOnCheckedChangeListener(this);
        RDSwitch.setOnCheckedChangeListener(this);
        MQSwitch.setOnCheckedChangeListener(this);

        mAccountText = (TextView) rootView.findViewById(R.id.account_view);
        mRDText = (TextView) rootView.findViewById(R.id.rd_view);
        mMQText = (TextView) rootView.findViewById(R.id.mq_view);
        mScrollView = (ScrollView) rootView.findViewById(R.id.scroll_view);
        mActionLog = (TextView) rootView.findViewById(R.id.action_log_view);
        mResultLog = (TextView) rootView.findViewById(R.id.result_log_view);

        Button setIPButton = (Button) rootView.findViewById(R.id.setip_button);
        Button signUpButton = (Button) rootView.findViewById(R.id.signup_button);
        Button signInButton = (Button) rootView.findViewById(R.id.signin_button);
        Button signOutButton = (Button) rootView.findViewById(R.id.signout_button);
        Button createGroupButton = (Button) rootView.findViewById(R.id.creategroup_button);
        Button inviteButton = (Button) rootView.findViewById(R.id.invite_button);
        Button leaveGroupButton = (Button) rootView.findViewById(R.id.leavegroup_button);
        setIPButton.setOnClickListener(this);
        signUpButton.setOnClickListener(this);
        signInButton.setOnClickListener(this);
        signOutButton.setOnClickListener(this);
        createGroupButton.setOnClickListener(this);
        inviteButton.setOnClickListener(this);
        leaveGroupButton.setOnClickListener(this);

        Button rdPubButton = (Button) rootView.findViewById(R.id.rdpub_button);
        Button rdDelButton = (Button) rootView.findViewById(R.id.rddel_button);
        mDevicePresenceButton = (Button) rootView.findViewById(R.id.rddp_button);
        rdPubButton.setOnClickListener(this);
        rdDelButton.setOnClickListener(this);
        mDevicePresenceButton.setOnClickListener(this);

        Button mqBrokerButton = (Button) rootView.findViewById(R.id.mqget_button);
        Button createTopicButton = (Button) rootView.findViewById(R.id.mqcreate_button);
        Button subTopicButton = (Button) rootView.findViewById(R.id.mqsub_button);
        Button unsubTopicButton = (Button) rootView.findViewById(R.id.mqunsub_button);
        Button pubToicButton = (Button) rootView.findViewById(R.id.mqpub_button);
        mqBrokerButton.setOnClickListener(this);
        createTopicButton.setOnClickListener(this);
        subTopicButton.setOnClickListener(this);
        unsubTopicButton.setOnClickListener(this);
        pubToicButton.setOnClickListener(this);

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

}