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
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcRDClient;
import org.iotivity.base.OcAccountManager;

import static org.iotivity.service.ic.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class ICResourceDirectoryCommonAdapter extends Base implements OcRDClient.OnPublishResourceListener,
        OcRDClient.OnDeleteResourceListener, OcResource.OnObserveListener {

    public static boolean isOnPublishResourceCompleted = false;
    public static boolean onDeleteResourceCompleted = false;

    public ICResourceDirectoryCommonAdapter() {

    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1, int arg2) {
        showOutPut("Observe Completed.");
    }

    @Override
    public void onObserveFailed(Throwable arg0) {
        showOutPut("Observed Failed");
    }

    @Override
    public void onDeleteResourceCompleted(int resultCode) {
        showOutPut("onDeleteResourceCompleted, result is " + resultCode);
        onDeleteResourceCompleted = true;
    }

    public void onDeleteResourceFailed(Throwable arg0) {
        showOutPut("onDeleteResource failed");
        onDeleteResourceCompleted = true;
    }

    @Override
    public void onPublishResourceCompleted(OcRepresentation ocRepresentation) {
        showOutPut("onPublishResourceCompleted");
        isOnPublishResourceCompleted = true;
        for (OcRepresentation child : ocRepresentation.getChildren()) {
            try {
                System.out.println("\tPublished Resource URI : " + child.getValue("href"));
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onPublishResourceFailed(Throwable throwable) {
        showOutPut("onPublishResourceFailed has failed");
        isOnPublishResourceCompleted = true;
    }

}
