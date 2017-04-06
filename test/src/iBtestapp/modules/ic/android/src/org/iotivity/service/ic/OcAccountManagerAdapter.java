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
import android.util.Log;

import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcConnectivityType;
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
import org.iotivity.base.OcAccountManager;

import static org.iotivity.service.ic.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class OcAccountManagerAdapter extends Base
        implements OcAccountManager.OnPostListener, OnGetListener,
        OnDeleteListener, OnObserveListener {

    public static String  sInviteeUuid                  = null;
    public static String  sGroupId                      = null;
    public static boolean sIsObserveCompleted            = false;
    public static boolean sIs_onDeleteCompletedCBInvoked = false;
    public static boolean sIs_sendInvitationCBInvoked    = false;

    @Override
    public void onPostCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation) {
        showOutPut("createGroup or sendInviation were successful");
        sIs_sendInvitationCBInvoked = true;
        try {
            sGroupId = ocRepresentation.getValue("gid");
            showOutPut("\tgroupId: " + sGroupId);
            showOutPut("getGroupInfo");
        } catch (OcException e) {
            Log.e(TAG, e.toString());
        }
    }

    @Override
    public void onPostFailed(Throwable throwable) {
        showOutPut("Failed to createGroup");
        sIs_sendInvitationCBInvoked = true;
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
        }
    }

    @Override
    public void onDeleteCompleted(List<OcHeaderOption> arg0) {
        showOutPut("Delete device succeed");
        sIs_onDeleteCompletedCBInvoked = true;
    }

    @Override
    public void onDeleteFailed(Throwable arg0) {
        showOutPut("Delete device failed");
    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation, int i) {
        showOutPut("observeGroup was successful");
        sIsObserveCompleted = true;
        try {
            if (REGISTER == i) {
                System.out.println("REGISTER was successful");
            } else if (DEREGISTER == i) {
                System.out.println("DEREGISTER was successful");
            } else {
                String gid = ocRepresentation.getValue("gid");
                System.out.println("\tGroupID: " + gid);

                String gmid = ocRepresentation.getValue("gmid");
                System.out.println("\tGroupMasterID: " + gmid);

                String[] midlist = ocRepresentation.getValue("midlist");
                if (midlist == null || midlist.length == 0) {
                    System.out.println("\tMember List is empty");
                } else {
                    System.out.println("\tMember List(" + midlist.length + ")");
                    for (String mid : midlist) {
                        System.out.println("\t : " + mid);
                    }
                }

                String[] dilist = ocRepresentation.getValue("dilist");
                if (dilist == null || dilist.length == 0) {
                    System.out.println("\tDevice List is empty");
                } else {
                    System.out.println("\tDevice List(" + dilist.length + ")");
                    for (String di : dilist) {
                        System.out.println("\t : " + di);
                    }
                }
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onObserveFailed(Throwable throwable) {
        showOutPut("Failed to observeGroup");
        sIsObserveCompleted = true;
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            System.out.println("Error code: " + errCode);
        }
    }

    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation) {
        showOutPut("searchUser was successful");
        try {
            OcRepresentation[] userList = ocRepresentation.getValue("ulist");
            for (OcRepresentation user : userList) {
                sInviteeUuid = user.getValue("uid");
                OcRepresentation userInfo = user.getValue("uinfo");
                String inviteeUserId = userInfo.getValue("userid");
                Log.d(TAG, "inviteeUserId : " + inviteeUserId);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    public synchronized void onGetFailed(Throwable throwable) {
        showOutPut("Failed to searchUser");
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            System.out.println("Error code: " + errCode);
        }
    }
}