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
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.ErrorCode;

import static org.iotivity.service.ic.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class OconGetGroupInfoAllAdapter extends Base implements OnGetListener{
    public static String sGroupId = null;
    public static boolean sGroupInfoCallbackinvoked = false;
    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list,
                                            OcRepresentation ocRepresentation) {
         showOutPut("getGroupInfoAll was successful");
         sGroupInfoCallbackinvoked = true;
         try {

             OcRepresentation[] gidlist = ocRepresentation.getValue("groups");
             if (gidlist == null || gidlist.length == 0) {
                 System.out.println("\tgroup list is empty");
                 sGroupId = null;
             } else {
                 System.out.println("\tgroup list");

                 for (OcRepresentation group : gidlist) {
                     String gid = group.getValue("gid");
                     String gname = group.getValue("gname");
                     String owner = group.getValue("owner");

                     System.out.println("\t\t-GroupID : " + gid);
                     System.out.println("\t\t Group name : " + gname);
                     System.out.println("\t\t Owner : " + owner);

                     String[] members = group.getValue("members");
                     if (members != null && members.length != 0) {
                         System.out.println("\t\t members :");
                         for (String member : members) {
                             System.out.println("\t\t\t" + member);
                         }
                     }

                     String[] devices = group.getValue("devices");
                     if (devices != null && devices.length != 0) {
                         System.out.println("\t\t devices");
                         for (String device : devices) {
                             System.out.println("\t\t\t" + device);
                         }
                     }

                     if (group.hasAttribute("parent")) {
                         System.out.println("\t\t parent group : " + group.getValue("parent"));
                     }

                     sGroupId = gid;
                 }
                 System.out.println("\tcurrent group is " + sGroupId);
             }
         } catch (OcException e) {
             e.printStackTrace();
         }
     }

      @Override
      public void onGetFailed(Throwable throwable) {
          showOutPut("Failed to getGroupInfoAll");
          if (throwable instanceof OcException) {
              OcException ocEx = (OcException) throwable;
              Log.e(TAG, ocEx.toString());
              ErrorCode errCode = ocEx.getErrorCode();
              System.out.println("Error code: " + errCode);
          }
      }
}