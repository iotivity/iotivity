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
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.service.rc;

import org.iotivity.service.rc.RCTestAppUtilityFunctions;
import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.testapp.framework.MenuInfo;

import android.os.Bundle;

import java.util.LinkedHashMap;
import java.util.Map;

public class MainActivity extends Base {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Map<String, MenuInfo> menuMap = new LinkedHashMap<String, MenuInfo>();
        menuMap.put("1", new MenuInfo("Start Container", "startContainer"));
        menuMap.put("2", new MenuInfo("Add Bundle", "addBundle"));
        menuMap.put("3", new MenuInfo("Start Bundle", "startBundle"));
        menuMap.put("4", new MenuInfo("Add Resources", "addResources"));
        menuMap.put("5", new MenuInfo("Remove Resources", "removeResources"));
        menuMap.put("6", new MenuInfo("Stop Bundle", "stopBundle"));
        menuMap.put("7", new MenuInfo("Remove Bundle", "removeBundle"));
        menuMap.put("8", new MenuInfo("Display Current Bundle List",
                "displayCurrentBundleList"));
        menuMap.put("9", new MenuInfo("Display Current Resource List",
                "displayCurrentResourceList"));
        menuMap.put("10", new MenuInfo("Stop Container", "stopContainer"));
        menuMap.put("11", new MenuInfo("Clear Log", "clearLog"));
        menuMap.put("12", new MenuInfo("Register Resources",
                "registerResources"));
        menuMap.put("13", new MenuInfo(
                "UnRegister Resources\n*********** Client Menu ***********",
                "unRegisterResources"));
        menuMap.put("14", new MenuInfo("Observe Resource Container",
                "observeResourceContainer"));
        menuMap.put("15", new MenuInfo("Exit", "exitTestApp"));
        RegisterApp("Resource Container", menuMap, new Action());

        RCTestAppUtilityFunctions.rcConfigurePlatform(getContext());
        RCTestAppUtilityFunctions.copyFilesToDevice(getContext());
    }
}
