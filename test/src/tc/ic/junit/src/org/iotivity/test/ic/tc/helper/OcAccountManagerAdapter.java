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

package org.iotivity.test.ic.tc.helper;

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
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.ErrorCode;

import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;

public class OcAccountManagerAdapter implements OnPostListener, OnGetListener,
        OnDeleteListener, OnObserveListener {

    public static String  sInviteeUuid                  = null;
    public static String  sGroupId                      = null;
    public static boolean sIsObserveCompleted           = false;
    public static boolean sIsonDeleteCompletedCBInvoked = false;
    public static boolean sIssendInvitationCBInvoked    = false;
    public static boolean onGetCompletedOk              = false;

    @Override
    public void onPostCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation) {
        Log.i(TAG,
                "createGroup or sendInviation or adProertyvalue were successful");
        sIssendInvitationCBInvoked = true;
        try {
            sGroupId = ocRepresentation.getValue("gid");
            Log.d("\tgroupId: ", sGroupId);
        } catch (OcException e) {
            Log.e(TAG, e.toString());
        }
    }

    @Override
    public void onPostFailed(Throwable throwable) {
        Log.e(TAG,
                "Failed to createGroup/sendInovation/add/delete/updatepropertyGroup");
        sIssendInvitationCBInvoked = false;
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
        }
    }

    @Override
    public void onDeleteCompleted(List<OcHeaderOption> arg0) {
        Log.i(TAG, "successed to delete operation");
        sIsonDeleteCompletedCBInvoked = true;
    }

    @Override
    public void onDeleteFailed(Throwable arg0) {
        Log.e(TAG, "failed to delete operation");
        sIsonDeleteCompletedCBInvoked = false;
    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation, int i) {
        System.out.println("observeGroup was successful");
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
        System.out.println("Failed to observeGroup");
        sIsObserveCompleted = false;
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
        System.out.println("searchUser was successful");
        // try {
        // OcRepresentation[] userList = ocRepresentation.getValue("ulist");
        // for (OcRepresentation user : userList) {
        // sInviteeUuid = user.getValue("uid");
        // OcRepresentation userInfo = user.getValue("uinfo");
        // String inviteeUserId = userInfo.getValue("userid");
        // Log.d(TAG, "inviteeUserId : " + inviteeUserId);
        onGetCompletedOk = true;
        Log.i(TAG, "onGetCompletedOk value is " + onGetCompletedOk);

        // } catch (OcException e) {
        // e.printStackTrace();
        // }
    }

    @Override
    public synchronized void onGetFailed(Throwable throwable) {
        System.out.println("Failed to searchUser");
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            System.out.println("Error code: " + errCode);
        }
    }
}
