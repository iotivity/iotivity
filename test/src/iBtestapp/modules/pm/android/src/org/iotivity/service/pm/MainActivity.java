/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
package org.iotivity.service.pm;

import android.os.Bundle;

import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.testapp.framework.MenuInfo;

import java.util.LinkedHashMap;
import java.util.Map;

public class MainActivity extends Base {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Map<String, MenuInfo> menuMap = new LinkedHashMap<String, MenuInfo>();
        menuMap.put("11", new MenuInfo("Start JustWorks Server 1",
                "startJustWorksServer1"));
        menuMap.put("12", new MenuInfo("Start JustWorks Server 2",
                "startJustWorksServer2"));
        menuMap.put("13", new MenuInfo("Start Random Pin Server",
                "startRandomPinServer"));
        menuMap.put("21",
                new MenuInfo("Start Provisioning Client", "startPmClient"));
        menuMap.put("22", new MenuInfo("Justworks Ownership Transfer",
                "justworksOwnerShipTransfer"));
        menuMap.put("23", new MenuInfo("RandomPin Ownership Transfer",
                "randomPinOwnerShipTransfer"));
        menuMap.put("24", new MenuInfo("Provision Access Control List (ACL)",
                "provisionACL"));
        menuMap.put("25", new MenuInfo("Provision Credential",
                "provisionCredential"));
        menuMap.put("26", new MenuInfo("Provision Pairwise Devs",
                "provisionPairwiseDev"));
        menuMap.put("27", new MenuInfo("Provision Direct Pairing",
                "provisionDirectPairing"));
        menuMap.put("28", new MenuInfo("Get Linked Devices", "getLinkedDev"));
        menuMap.put("29", new MenuInfo("Unlink Devices", "unlinkDev"));
        menuMap.put("30", new MenuInfo("Remove Devices", "removeDev"));

        menuMap.put("41", new MenuInfo("Start Direct Pairing Client",
                "startDpClient"));
        menuMap.put("42", new MenuInfo("Discover Direct Pairing Devs",
                "discoverDpDevs"));
        menuMap.put("43", new MenuInfo("Do Direct Pairing", "doDirectPairing"));
        menuMap.put("44", new MenuInfo("Get Direct Pairing Devs",
                "getDirectPairedDev"));

        // For Debugging Purpose
        menuMap.put("1", new MenuInfo("Discover Unowned Devices",
                "discoverUnowendDev"));
        menuMap
                .put("2", new MenuInfo("Discover Owned Devices", "discoverOwendDev"));
        menuMap.put("0", new MenuInfo("Clear Log", "clearLog"));

        RegisterApp("Provisioning Manager", menuMap, new Action(
                getApplicationContext()));

        showOutPut("Test");
    }

}
