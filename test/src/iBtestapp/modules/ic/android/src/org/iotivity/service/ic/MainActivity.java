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

import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import android.util.Log;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;

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

import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.testapp.framework.MenuInfo;
import static org.iotivity.service.ic.ICUtility.*;

public class MainActivity extends Base {
    private Activity mActivity;
    private Context mContext;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = this;
        mContext = mActivity.getBaseContext();
        ICAction.initOcPlatform(ModeType.CLIENT_SERVER, mActivity, mContext);

        Map<String, MenuInfo> menuMap = new LinkedHashMap<String, MenuInfo>();
        menuMap.put("1", new MenuInfo("Sign Up With Controlee", "signUpWithControlee"));
        menuMap.put("2", new MenuInfo("Sign In to Cloud", "signIntoCloud"));
        menuMap.put("3", new MenuInfo("Sign Out from Cloud", "signOutfromCloud"));
        menuMap.put("4", new MenuInfo("Search User Information With DEV UUID", "searchUserInformationWithDevUUID"));
        menuMap.put("5", new MenuInfo("Delete Device", "deleteDevice"));
        menuMap.put("6", new MenuInfo("Create Group", "createGroup"));
        menuMap.put("7", new MenuInfo("Delete Group", "deleteGroup"));
        menuMap.put("8", new MenuInfo("Get Group Info ALL", "getGroupInfoALL"));
        menuMap.put("9", new MenuInfo("Get Group Info", "getGroupInfo"));
        menuMap.put("10", new MenuInfo("Observe Group", "observeGroup"));
        menuMap.put("11", new MenuInfo("Cancel Observe Group", "cancelObserveGroup"));
        menuMap.put("12", new MenuInfo("Observe Invitation", "observeInvitation"));
        menuMap.put("13", new MenuInfo("Cancel Observe Invitation", "cancelObserveInvitation"));
        menuMap.put("14", new MenuInfo("Send Invitation", "sendInvitation"));
        menuMap.put("15", new MenuInfo("Cancel Invitation", "cancelInvitation"));
        menuMap.put("16", new MenuInfo("Delete Invitation", "deleteInvitation"));
        menuMap.put("17", new MenuInfo("Publish Resource", "publishResource"));
        // menuMap.put("====================Controller===========================
        menuMap.put("20", new MenuInfo("Sign Up With Controller", "signUpWithController"));
        menuMap.put("21", new MenuInfo("Sign In With Controller", "signInWithController"));
        menuMap.put("22", new MenuInfo("Sign Out from Cloud", "signOutfromCloudController"));
        menuMap.put("23", new MenuInfo("Add property value to group", "addpropertyvaluetogroup"));
        menuMap.put("24", new MenuInfo("Delete property value to group ", "deletepropertyvaluetogroup"));
        menuMap.put("25", new MenuInfo("Update property value to group ", "updatepropertyvaluetogroup"));
        menuMap.put("26", new MenuInfo("Send DELETE Request to resource", "sendDeleteRequesttoresource"));
        // ====================Subscribe Features===========================
        menuMap.put("30", new MenuInfo("Discover all topics", "discoverallTopics"));
        menuMap.put("31", new MenuInfo("Create topic", "createTopic"));
        menuMap.put("32", new MenuInfo("Subscribe to selected topic", "subscribeTopic"));
        menuMap.put("33", new MenuInfo("Unsubscribe to selected topic", "unsubscribeTopic"));
        menuMap.put("34", new MenuInfo("Publish topic", "publishTopic"));
        menuMap.put("0", new MenuInfo("Exit", "Exit"));

        RegisterApp("Iotivity Cloud", menuMap, new ICAction());
    }

}
