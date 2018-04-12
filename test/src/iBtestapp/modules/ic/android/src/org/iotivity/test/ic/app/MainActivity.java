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
import static org.iotivity.test.ic.app.ICUtility.*;

public class MainActivity extends Base {
    private Activity mActivity;
    public static Context sContext;
    private ICAction mICAction = new ICAction(); 
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = this;
        sContext = mActivity.getBaseContext();
        mICAction.initOcPlatform(ModeType.CLIENT_SERVER, mActivity, sContext);
        mICAction.initOcPlatformForConroller(ModeType.CLIENT_SERVER, mActivity, sContext);

        Map<String, MenuInfo> menuMap = new LinkedHashMap<String, MenuInfo>();
     // menuMap.put("====================Controlee===========================
        menuMap.put("101", new MenuInfo("Start Controlee", "startControlee"));
        menuMap.put("102", new MenuInfo("Sign Up With Controlee", "signUpWithControlee"));
        menuMap.put("103", new MenuInfo("Sign In with Controlee", "signIntoCloud"));
        menuMap.put("104", new MenuInfo("Sign Out with Controlee", "signOutfromCloud"));
        menuMap.put("105", new MenuInfo("Create Group", "createGroup"));
        menuMap.put("106", new MenuInfo("Create Group with Query", "createGroupwithQuery"));
        menuMap.put("107", new MenuInfo("Send Invitation", "sendInvitation"));
        menuMap.put("108", new MenuInfo("Publish Resource", "publishResource"));
        menuMap.put("109", new MenuInfo("Trougle Resource attribute from controlee", "publishTrougleResource"));
     // menuMap.put("====================Controller===========================  
        menuMap.put("201", new MenuInfo("Start Controller", "startController"));
        menuMap.put("202", new MenuInfo("Sign Up With Controller", "signUpWithController"));
        menuMap.put("203", new MenuInfo("Sign In With Controller", "signInWithController"));
        menuMap.put("204", new MenuInfo("Sign Out from Cloud", "signOutfromCloudController"));
        menuMap.put("205", new MenuInfo("Search User Information With DEV UUID", "searchUserInformationWithDevUUID"));
        menuMap.put("206", new MenuInfo("Search User Information With Phone Number", "searchUserWithPhone"));
        menuMap.put("207", new MenuInfo("Search User Information With Email", "searchUserWithEmail"));
        menuMap.put("208", new MenuInfo("Delete Device", "deleteDevice"));
        menuMap.put("209", new MenuInfo("Delete Group", "deleteGroup"));
        menuMap.put("210", new MenuInfo("Get Group Info ALL", "getGroupInfoALL"));
        menuMap.put("211", new MenuInfo("Get Group Info", "getGroupInfo"));
        menuMap.put("212", new MenuInfo("Observe Group", "observeGroup"));
        menuMap.put("213", new MenuInfo("Cancel Observe Group", "cancelObserveGroup"));
        menuMap.put("214", new MenuInfo("Observe Invitation", "observeInvitation"));
        menuMap.put("215", new MenuInfo("Cancel Observe Invitation", "cancelObserveInvitation"));
        menuMap.put("216", new MenuInfo("Cancel Invitation", "cancelInvitation"));
        menuMap.put("217", new MenuInfo("Delete Invitation", "deleteInvitation"));
        menuMap.put("218", new MenuInfo("Find Resource with Query", "findResouce"));
        menuMap.put("219", new MenuInfo("Find Resource without Query", "findResouceWithOutQurey"));
        menuMap.put("220", new MenuInfo("Trougle Resource attribute from controller", "trougleResource"));
        menuMap.put("221", new MenuInfo("Add property value to group", "addpropertyvaluetogroup"));
        menuMap.put("222", new MenuInfo("Delete property value to group ", "deletepropertyvaluetogroup"));
        menuMap.put("223", new MenuInfo("Update property value to group ", "updatepropertyvaluetogroup"));
        menuMap.put("224", new MenuInfo("Send DELETE Request to resource", "sendDeleteRequesttoresource"));
        // ===================MQ Subscribe Features===========================
        menuMap.put("301", new MenuInfo("Create topic", "createTopic"));
        menuMap.put("302", new MenuInfo("Create type based topic", "createTypedTopic"));
        menuMap.put("303", new MenuInfo("Discover all topics", "discoverallTopics"));
        menuMap.put("304", new MenuInfo("Discover type based topics", "discoverTypedTopics"));
        menuMap.put("305", new MenuInfo("Select topic index for publishing", "seletcIndex"));
        menuMap.put("306", new MenuInfo("Publish topic", "publishTopic"));
//        ====================MQ Subscribe Features===========================
        menuMap.put("401", new MenuInfo("Discover all topics", "discoverallTopics"));
        menuMap.put("402", new MenuInfo("Discover type based topics", "discoverallTopics"));
        menuMap.put("305", new MenuInfo("Select topic index for sub/unsubscribe", "selectIndexforsubUnsub"));
        menuMap.put("403", new MenuInfo("Subscribe to selected topic", "subscribeTopic"));
        menuMap.put("404", new MenuInfo("Unsubscribe to selected topic", "unsubscribeTopic"));
        menuMap.put("0", new MenuInfo("Exit", "Exit"));

        RegisterApp("Iotivity Cloud", menuMap, new ICAction());
    }

}
