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
import org.iotivity.base.OcResource.*;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcException;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;
import org.iotivity.base.ModeType;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcAccountManager;

import static org.iotivity.service.ic.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class ICMessageAdapter extends Base implements OnMQTopicCreatedListener,
        OnMQTopicFoundListener, OnPostListener, OnObserveListener {

    public ICMessageAdapter() {

    }

    public static OcResource       sCurrentTopicResource   = null;
    public static boolean          sOnTopicResourceCreated = false;
    public static boolean          sOnTopicDiscoveried     = false;
    public static String           sResourceUri            = null;
    public static boolean          sOnPostCompleted        = false;
    public static boolean          sOnObserveCompleted     = false;
    public static OcRepresentation sCurrentResourceRep     = null;

    @Override
    public void onTopicResourceCreated(OcResource m_myResource) {
        showOutPut("onTopicResourceCreated");
        sOnTopicResourceCreated = true;
        sCurrentTopicResource = m_myResource;

        showOutPut(
                "onTopicResourceCreated : " + sCurrentTopicResource.getUri());
    }

    @Override
    public void onCreateTopicFailed(Throwable ex, String m_uri) {
        showOutPut("onTopicResourceCreate failed");
    }

    @Override
    public void onTopicDiscoveried(OcResource m_myResource) {
        showOutPut("Topic Discovered");
        sOnTopicResourceCreated = false;
        sResourceUri = m_myResource.getUri();
    }

    @Override
    public void onDiscoveryTopicFailed(Throwable ex, String m_uri) {
        showOutPut("Discovery Topic Failed.");
    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> m_headerOptionList,
            OcRepresentation m_ocRepresentation) {
        showOutPut("Topic Post Completed.");
        sOnPostCompleted = true;
    }

    @Override
    public void onPostFailed(Throwable ex) {
        showOutPut("Topic Post Failed");
    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> m_headerOptionList,
            OcRepresentation m_ocRepresentation, int m_sequenceNumber) {
        showOutPut("Observed Successed.");
        sOnObserveCompleted = true;
        sCurrentResourceRep = m_ocRepresentation;
    }

    @Override
    public void onObserveFailed(Throwable ex) {
        showOutPut("Observed Failed.");
    }

}
