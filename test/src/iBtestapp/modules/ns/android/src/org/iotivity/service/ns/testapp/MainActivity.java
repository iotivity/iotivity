//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package org.iotivity.service.ns.testapp;

import android.os.Bundle;
import android.util.Log;

import java.util.LinkedHashMap;
import java.util.Map;

import android.net.ConnectivityManager;
import android.app.AlertDialog;
import android.content.DialogInterface;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.testapp.framework.MenuInfo;
import static org.iotivity.service.ns.testapp.NSAppUtility.*;

public class MainActivity extends Base {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!isWifiConnected()) {
            showWifiUnavailableDialog();
            return;
        }

        configurePlatform();
        Map<String, MenuInfo> menuMap = new LinkedHashMap<String, MenuInfo>();

        menuMap.put("0", new MenuInfo("Exit", "exitApplication"));

        // Provider Menu Item
        menuMap.put("#1", new MenuInfo(" *** Provider Part ***", "dummyFunction"));
        menuMap.put("101", new MenuInfo("Start Provider (Provider Acceptor)", "startProvider"));
        menuMap.put("102", new MenuInfo("Start Provider (Consumer Acceptor)", "startProviderForConsumer"));
        menuMap.put("103", new MenuInfo("Stop Provider", "stopProvider"));
        menuMap.put("104", new MenuInfo("Accept Subscriptions", "acceptSubscription"));
        menuMap.put("105", new MenuInfo("Reject Subscriptions", "rejectSubscription"));
        menuMap.put("106", new MenuInfo("Send Notification", "sendNotification"));
        menuMap.put("107", new MenuInfo("Send Sync Info", "sendProviderSyncInfo"));
        menuMap.put("108", new MenuInfo("Add Topic", "addTopic"));
        menuMap.put("109", new MenuInfo("Delete Topic", "deleteTopic"));
        menuMap.put("110", new MenuInfo("Select Topic", "selectTopic"));
        menuMap.put("111", new MenuInfo("Unselect Topic", "unselectTopic"));
        menuMap.put("112", new MenuInfo("Consumer Topics", "consumerTopics"));
        menuMap.put("113", new MenuInfo("Show Topics", "showTopics"));

        // Consumer Menu Item
        menuMap.put("#2", new MenuInfo(" *** Consumer Part ***", "dummyFunction"));
        menuMap.put("201", new MenuInfo("Start Consumer", "startConsumer"));
        menuMap.put("202", new MenuInfo("Stop Consumer", "stopConsumer"));
        menuMap.put("203", new MenuInfo("Subscribe Consumer", "subscribeConsumer"));
        menuMap.put("204", new MenuInfo("Manual Scan for Provider", "rescanProvider"));
        menuMap.put("205", new MenuInfo("Send Sync Info", "sendConsumerSyncInfo"));
        menuMap.put("206", new MenuInfo("Get Topics", "getTopics"));
        menuMap.put("207", new MenuInfo("Update Topics", "updateTopics"));

        RegisterApp("Notification Service", menuMap, new NSAppAction());
    }

    private boolean isWifiConnected() {
        ConnectivityManager connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
        return connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI)
                .isConnected();
    }

    private void showWifiUnavailableDialog() {
        new AlertDialog.Builder(this)
                .setTitle("Error")
                .setMessage(
                        "WiFi is not enabled/connected! Please connect the WiFi and start application again...")
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                }).create().show();
    }

    private void configurePlatform() {
        OcPlatform.Configure(new PlatformConfig(getApplicationContext(),
                ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0,
                QualityOfService.LOW));

        Log.i(TAG, "Configuration done Successfully");

        try {
            OcPlatform.stopPresence();
        } catch (Exception e) {
            Log.d(TAG, "Can't stop presence...");
        }
    }
}
