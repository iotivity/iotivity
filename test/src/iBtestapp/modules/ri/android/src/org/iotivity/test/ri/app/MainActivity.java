/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 * <p>
 * <p>
 * <p>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.test.ri.app;

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

public class MainActivity extends Base {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Map<String, MenuInfo> menuMap = new LinkedHashMap<String, MenuInfo>();
        menuMap.put("0", new MenuInfo("Quit InerOp App", "exitApplication"));
        menuMap.put("100", new MenuInfo("Set CON Type Messaging", "setHighQoS"));
        menuMap.put("101", new MenuInfo("Set NON Type Messaging", "setLowQoS"));
        menuMap.put("102", new MenuInfo("Select IP Transport- IPv4", "setIPv4"));
        menuMap.put("103", new MenuInfo("Select IP Transport - IPv6", "setIPv6"));
        menuMap.put("104", new MenuInfo("Select IP Transport - Both IPv6 & IPv4", "setIPBoth"));
        menuMap.put("105", new MenuInfo("Select BLE Transport", "setBLE"));
        menuMap.put("106", new MenuInfo("Select EDR Transport", "setEDR"));
        menuMap.put("107", new MenuInfo("Select TCP Transport", "setTCP"));
        menuMap.put("108", new MenuInfo("Select NFC Transport", "setNFC"));
        menuMap.put("1", new MenuInfo("Create Non-Secured Air Conditioner Device", "createNonSecuredAirCon"));
        menuMap.put("2", new MenuInfo("Create Secured Air Conditioner Device",
                "createSecuredAirCon"));
        menuMap.put("3", new MenuInfo("Delete All Resources", "deleteResource"));
        menuMap.put("4", new MenuInfo("Update Local Resource", "updateLocalResource"));
        menuMap.put("10", new MenuInfo("Discover Device - Multicast", "discoverDeviceMulticast"));
        menuMap.put("11", new MenuInfo("Discover Platform - Multicast", "discoverPlatformMulticast"));
        menuMap.put("12", new MenuInfo("Discover Resources - Multicast", "findAllResources"));
        menuMap.put("13", new MenuInfo("Discover Resources - Unicast", "findAllResourceUnicast"));
        menuMap.put("14", new MenuInfo("Discover Resources - Multicast with Query", "findResourceWithType"));
        menuMap.put("15", new MenuInfo("Send GET Request to Vertical Resources", "sendGetRequestVertical"));
        menuMap.put("16", new MenuInfo("Send GET Request to Core Resources", "sendGetRequestCore"));
        menuMap.put("17", new MenuInfo("Send PUT Request to Vertical Resources", "sendPutRequestVertical"));
        menuMap.put("18", new MenuInfo("Send PUT Request to Core Resources", "sendPutRequestCore"));
        menuMap.put("19", new MenuInfo("Send POST Request to Vertical Resources", "sendPostRequestVertical"));
        menuMap.put("20", new MenuInfo("Send POST Request to Core Resources", "sendPostRequestCore"));
        menuMap.put("21", new MenuInfo("Send Delete Request to Vertical Resources", "sendDeleteRequestVertical"));
        menuMap.put("22", new MenuInfo("Send Delete Request to Core Resources", "sendDeleteRequestCore"));
        menuMap.put("23", new MenuInfo("Send Observe Request to Vertical Observable Resources", "sendObserveRequestVertical"));
        menuMap.put("24", new MenuInfo("Send Observe Request to Core Resources", "sendObserveRequestCore"));
        menuMap.put("25", new MenuInfo("Send Observe Request to Vertical Non-observable Resources", "sendObserveRequestNonObservable"));
        menuMap.put("26", new MenuInfo("Cancel Observing Resource", "cancelObserveResource"));
        menuMap.put("27", new MenuInfo("Send POST Request with Blockwise Payload", "sendPostRequestBlockwise"));
        menuMap.put("28", new MenuInfo("Get Input From User", "getUserInput"));

        RegisterApp("Resource Introspection", menuMap, new RITestAppAction(getApplicationContext()));
    }

}