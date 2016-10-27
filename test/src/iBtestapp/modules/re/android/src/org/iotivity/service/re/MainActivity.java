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

package org.iotivity.service.re;

import android.os.Bundle;
import android.util.Log;

import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.testapp.framework.MenuInfo;

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

import static org.iotivity.service.re.REUtility.*;

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
        menuMap.put("1", new MenuInfo("Create Resource", "createResource"));
        menuMap.put("2", new MenuInfo("Set Attributes", "setAttributes"));
        menuMap.put("3", new MenuInfo("Get Attributes", "getAttributes"));
        menuMap.put("4", new MenuInfo("Discover Resource without query",
                "discoverResource"));
        menuMap.put("5", new MenuInfo("Discover Resource with Resource Type",
                "discoverResourceByType"));
        menuMap.put("6", new MenuInfo("Discover Non-Discoverable Resource",
                "discoverNonDiscoverableResource"));
        menuMap.put("7", new MenuInfo("Start Monitoring", "startMonitoring"));
        menuMap.put("8", new MenuInfo("Stop Monitoring", "stopMonitoring"));
        menuMap.put("9", new MenuInfo("Get State", "getState"));
        menuMap.put("10", new MenuInfo("Set Remote Attributes",
                "setRemoteAttributes"));
        menuMap.put("11", new MenuInfo("Get Remote Attributes",
                "getRemoteAttributes"));
        menuMap.put("12", new MenuInfo("Start Caching", "startCaching"));
        menuMap.put("13", new MenuInfo("Stop Caching", "stopCaching"));
        menuMap.put("14", new MenuInfo("Get Cached Attributes",
                "getCachedAttributes"));
        menuMap.put("15", new MenuInfo("Get Cached State", "getCachedState"));
        menuMap.put("0", new MenuInfo("Exit", "exitApplication"));

        RegisterApp("Resource Encapsulation", menuMap, new RETestAppAction());
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
    }
}