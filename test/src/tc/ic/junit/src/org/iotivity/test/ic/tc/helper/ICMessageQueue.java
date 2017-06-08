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

import java.util.List;
import java.util.EnumSet;
import java.lang.*;
import java.util.ArrayList;

import android.util.Log;
import android.content.Context;

import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatform.*;
import org.iotivity.base.OcException;
import org.iotivity.base.OcResource.OnObserveListener;
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

import org.iotivity.cloud.*;
import org.iotivity.configuration.IConfiguration;
import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;

public class ICMessageQueue implements  IConfiguration{
    private static final String OIC_IF_BASELINE = "oic.if.baseline";
    private static final String A_LIGHT = "/a/light";
    private static final String CORE_LIGHT = "core.light";
    public static final String RESOURCE_INTERFACE = OcPlatform.DEFAULT_INTERFACE;
    public static final String sMQHostAddress = OcAccountManagerHelper
            .getHostAddres(TLS.DISABLED);
    private boolean                mIsResourceFound = false;
    private OcResource             mResource;
    private static final String    RT              = "?rt=";
    public static OcRepresentation sRep             = new OcRepresentation();
    public OcRepresentation        mMsg             = new OcRepresentation();
    private boolean                mSwitchingFlag   = true;

    public void createResource(Context context) {
        PlatformConfig cfg = new PlatformConfig(context, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(
                    OcResourceRequest ocResourceRequest) {
                return EntityHandlerResult.OK;
            }
        };
        OcResourceHandle handler = null;
        try {
            handler = OcPlatform.registerResource(A_LIGHT, CORE_LIGHT,
                    OIC_IF_BASELINE, entityHandler,
                    EnumSet.of(ResourceProperty.DISCOVERABLE));
        } catch (OcException ex) {
        }
    }

    OcPlatform.OnResourceFoundListener listener = new OcPlatform.OnResourceFoundListener() {
        @Override
        public void onFindResourceFailed(Throwable throwable, String s) {
            mIsResourceFound = true;
        }

        @Override
        public void onResourceFound(OcResource ocResource) {
            mIsResourceFound = true;
            if (ocResource == null) {
            } else {
                mResource = ocResource;
                Log.d("Resource found", ocResource.getUri());
            }
            mIsResourceFound = true;
        }

    };

    public OcResource findClintResource(Context context) {
        String requestUri = OcPlatform.WELL_KNOWN_QUERY + RT + CORE_LIGHT;
        createResource(context);
        try {
            OcPlatform.findResource("", requestUri,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), listener);
            try {
                Thread.sleep(5000);
            } catch (InterruptedException ex) {
                Log.d("Wait not work", ex.getMessage());
            }
        } catch (OcException ex) {
            Log.d("Resource found", ex.getMessage());
        }
        return mResource;
    }

    public OcRepresentation generateRep() {
        try {
            if (mSwitchingFlag) {
                mMsg.setValue("blue", "on");
                mMsg.setValue("red", "off");
                mSwitchingFlag = false;
            } else {
                mMsg.setValue("blue", "off");
                mMsg.setValue("red", "on");
                mSwitchingFlag = true;
            }
            sRep.setValue("message", mMsg);
            return sRep;
        } catch (Exception ex) {
            Log.d("generateRep got crashed", ex.getMessage());
        }
        return null;
    }

    public void getMQBroker() {
        
        List<String> resourceTypeList = new ArrayList<>();
        List<String> resourceInterfaceList = new ArrayList<>();
        resourceInterfaceList.add(RESOURCE_INTERFACE);
        resourceTypeList.add("ocf.wk.ps");
        try {
            sMQbrokerResource = OcPlatform.constructResourceObject(
                    sMQHostAddress, MQ_BROKER_URI,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP,
                            OcConnectivityType.CT_IP_USE_V4),
                    false, resourceTypeList, resourceInterfaceList);

            System.out
                    .println("found MQ broker : " + sMQbrokerResource.getHost());
        } catch (OcException e) {
            e.printStackTrace();
        }
    }
}
