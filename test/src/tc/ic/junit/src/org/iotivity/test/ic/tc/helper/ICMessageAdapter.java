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

package org.iotivity.test.ic.tc.helper;

import java.util.*;
import java.util.ArrayList;

import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcResource.*;
import org.iotivity.test.ic.tc.helper.ICHelper;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.ModeType;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;

import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;
import static org.iotivity.test.ic.tc.helper.ICMessageQueue.*;
import org.iotivity.test.ic.tc.helper.ICMessageQueue;

public class ICMessageAdapter implements OnMQTopicCreatedListener,
        OnMQTopicFoundListener, OnPostListener, OnObserveListener {

    public ICMessageAdapter() {

    }
    public static OcResource       scurrentTopicResource    = null;
    public static boolean          sOnTopicResourceCreated = false;
    public static boolean          sOnTopicDiscoveried      = false;
    public static String           sResourceUri             = null;
    public static boolean          sOnPostCompleted        = false;
    public static boolean          sOnObserveCompleted     = false;
    public static OcRepresentation sCurrentResourceRep      = null;

    @Override
    public void onTopicResourceCreated(OcResource m_myResource) {
        System.out.println("onTopicResourceCreated");
        sOnTopicResourceCreated = true;
        scurrentTopicResource = m_myResource;

        System.out.println(
                "onTopicResourceCreated : " + scurrentTopicResource.getUri());
    }

    @Override
    public void onCreateTopicFailed(Throwable ex, String m_uri) {
        System.out.println("onTopicResourceCreate failed");
    }

    @Override
    public void onTopicDiscoveried(OcResource m_myResource) {
        sOnTopicResourceCreated = false;
        sResourceUri = m_myResource.getUri();
    }

    @Override
    public void onDiscoveryTopicFailed(Throwable ex, String m_uri) {
    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> m_headerOptionList,
            OcRepresentation m_ocRepresentation) {
        sOnPostCompleted = true;
    }

    @Override
    public void onPostFailed(Throwable ex) {
    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> m_headerOptionList,
            OcRepresentation m_ocRepresentation, int m_sequenceNumber) {
        sOnObserveCompleted = true;
        sCurrentResourceRep = m_ocRepresentation;
    }

    @Override
    public void onObserveFailed(Throwable ex) {
    }

}
