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
 *
 ******************************************************************/

package org.iotivity.test.cs.app;

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
        menuMap.put("100",
                new MenuInfo("Start Cloud Controlee", "startCloudControlee"));
        menuMap.put("101", new MenuInfo("Sing Up", "signUp"));
        menuMap.put("102", new MenuInfo("Sign In", "signIn"));
        menuMap.put("103", new MenuInfo("Sign Out", "signOut"));

        menuMap.put("104", new MenuInfo("OCCloudCertificateIssueRequest",
                "cloudCertificateIssueRequest"));
        menuMap.put("105", new MenuInfo("OCCloudGetCRL", "cloudGetCRL"));
        menuMap.put("106", new MenuInfo("OCCloudPostCRL", "cloudPostCRL"));

        menuMap.put("108", new MenuInfo("OCCloudGetAclIdByDevice",
                "cloudGetAclIdByDevice"));
        menuMap.put("109", new MenuInfo("OCCloudAclIndividualGetInfo",
                "cloudAclIndividualGetInfo"));

        RegisterApp("Cloud Provisioning Manager", menuMap,
                new Action(getApplicationContext()));
        showOutPut("Test");

    }
}
