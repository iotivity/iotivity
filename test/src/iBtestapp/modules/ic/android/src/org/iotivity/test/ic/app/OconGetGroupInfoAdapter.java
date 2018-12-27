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

import java.util.List;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;

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

import static org.iotivity.test.ic.app.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class OconGetGroupInfoAdapter extends Base implements OnGetListener{
    public static String sGroupId = null;
    public static boolean sGroupInfoCallbackinvoked = false;
    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list,
                                            OcRepresentation ocRepresentation) {
         showOutPut("getGroupInfo was successful");
         sGroupInfoCallbackinvoked = true;
         Map<String, Object> valueMap = ocRepresentation.getValues();

            for (Map.Entry<String, Object> entry : valueMap.entrySet()) {
                System.out.println("\tproperty: " + entry.getKey() + ":\t" + entry.getValue());
            }         
     }

      @Override
      public void onGetFailed(Throwable throwable) {
          showOutPut("Failed to getGroupInfo");
          if (throwable instanceof OcException) {
              OcException ocEx = (OcException) throwable;
              Log.e(TAG, ocEx.toString());
              ErrorCode errCode = ocEx.getErrorCode();
              System.out.println("Error code: " + errCode);
          }
      }
}
