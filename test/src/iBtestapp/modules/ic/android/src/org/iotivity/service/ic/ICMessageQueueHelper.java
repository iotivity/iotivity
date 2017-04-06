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
import java.util.EnumSet;
import java.util.LinkedList;
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

import static org.iotivity.service.ic.ICUtility.*;
import org.iotivity.service.testapp.framework.Base;

public class ICMessageQueueHelper extends Base {
    public static OcResource             sMQbrokerResource         = null;
    private static OcResourceHandle      slocalLightResourceHandle = null;
    public static List<OcResourceHandle> sResourceHandleList       = new LinkedList<>();
    private boolean                      mIsResourceFound          = false;
    private OcResource                   mResource;
    private static final String          RT                        = "?rt=";
    public static OcRepresentation       sRep                      = new OcRepresentation();
    public OcRepresentation              mMsg                      = new OcRepresentation();
    private boolean                      mSwitchingFlag            = true;

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
            handler = OcPlatform.registerResource("/a/light", "core.light",
                    "oic.if.baseline", entityHandler,
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
        String requestUri = OcPlatform.WELL_KNOWN_QUERY + RT + "core.light";

        createResource(context);

        try {
            OcPlatform.findResource("", requestUri,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), listener);

            try {
                Thread.sleep(TIMEOUT);
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

    public static boolean createResource() {
        if (slocalLightResourceHandle == null) {
            try {
                slocalLightResourceHandle = OcPlatform.registerResource(
                        Resource_URI, // resource URI
                        RESOURCE_TYPE, // resource type name
                        RESOURCE_INTERFACE, // using default interface
                        null, // use default entity handler
                        EnumSet.of(ResourceProperty.DISCOVERABLE,
                                ResourceProperty.OBSERVABLE));
                sResourceHandleList.add(slocalLightResourceHandle);
                showOutPut("Create Local Resource is success.");
                return true;
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }
        return false;
    }

    public void getMQBroker() {
        List<String> resourceTypeList = new ArrayList<>();
        List<String> resourceInterfaceList = new ArrayList<>();
        resourceInterfaceList.add(OcPlatform.DEFAULT_INTERFACE);
        resourceTypeList.add("ocf.wk.ps");
        try {
            sMQbrokerResource = OcPlatform.constructResourceObject(
                    IC_HOST_ADDRESS, MQ_BROKER_URI,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP,
                            OcConnectivityType.CT_IP_USE_V4),
                    false, resourceTypeList, resourceInterfaceList);

            showOutPut("found MQ broker : " + sMQbrokerResource.getHost());
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

}
