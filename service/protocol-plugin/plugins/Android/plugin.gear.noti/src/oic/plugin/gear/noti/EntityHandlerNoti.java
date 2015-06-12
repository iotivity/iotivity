//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file EntityHandlerNoti.java

package oic.plugin.gear.noti;

import java.util.EnumSet;
import java.util.UUID;

import oic.plugin.gear.noti.Activator.TemplateTypes;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.osgi.framework.BundleContext;

import android.util.Log;

public class EntityHandlerNoti implements OcPlatform.EntityHandler {

    final private static String TAG = "EntityHandler";

    public static String        textNoti;

    public EntityHandlerResult handleEntity(OcResourceRequest resourcerequest) {
        Log.e(TAG, "Entity Handler callback. LJH");

        System.out.println("IN Server CPP entity Handler");
        System.out.println("gear");
        // OcResourceResponse response = new OcResourceResponse();
        // int result = 1; // EH_ERROR
        if (resourcerequest != null
                && resourcerequest.getResourceUri().equals("/a/galaxy/gear")) {
            RequestType requestType = resourcerequest.getRequestType();
            EnumSet<RequestHandlerFlag> handlerFlagSet = resourcerequest
                    .getRequestHandlerFlagSet();

            if(handlerFlagSet.contains(RequestHandlerFlag.INIT))
            {
                Log.e(TAG, "requestFlag : Init");
            }
            if(handlerFlagSet.contains(RequestHandlerFlag.REQUEST))
            {
                OcResourceResponse response = new OcResourceResponse();
                OcRepresentation representation = new OcRepresentation();
                response.setRequestHandle(resourcerequest
                        .getRequestHandle());
                response.setResourceHandle(resourcerequest
                        .getResourceHandle());
                switch (requestType) {
                    case GET:
                        break;
                    case PUT:
                    try {
                        textNoti = resourcerequest
                                .getResourceRepresentation()
                                .getValue("power");
                    } catch (OcException e) {
                        // TODO Auto-generated catch block
                        Log.e(TAG, e.getMessage());
                    }
                        perform(0);
                        break;
                    case POST:
                        break;
                }
                response.setErrorCode(200);
                // representation.setUri("/a/galaxy/gear");
                try {
                    representation.setValue("name",
                            Activator.myNotify.m_name);
                    representation.setValue("power",
                            Activator.myNotify.m_power);
                    representation.setValue("brightness", 0);
                    representation.setValue("color", 0);
                    response.setResourceRepresentation(representation);
                } catch (OcException e) {
                    // TODO Auto-generated catch block
                    Log.e(TAG, e.getMessage());
                }
                
                try {
                    OcPlatform.sendResponse(response);
                } catch (OcException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                    return EntityHandlerResult.ERROR;
                }
                }
            if(handlerFlagSet.contains(RequestHandlerFlag.OBSERVER))
            {
                Log.e(TAG, "requestFlag : Observer");
            }
            return EntityHandlerResult.OK;
        }
        return EntityHandlerResult.ERROR;
        /*
         * if((requestFlag & 1 << 0) != 0 ) // init flag { Log.e(TAG,
         * "requestFlag : Init"); } if((requestFlag & 1 << 1) != 0)
         * //RequestFlag { if(requestType.equals("GET")) {
         * 
         * } else if(requestType.equals("PUT")) { textNoti =
         * resourcerequest.getResourceRepresentation().getValueString("power");
         * perform(0); } else if(requestType.equals("POST")) { }
         * OcRepresentation representation = new OcRepresentation();
         * OcResourceResponse response = new OcResourceResponse();
         * response.setRequestHandle(resourcerequest.getRequestHandle());
         * response.setResourceHandle(resourcerequest.getResourceHandle());
         * representation.setUri("/a/galaxy/gear");
         * representation.setValueString("name", Activator.myNotify.m_name);
         * representation.setValueString("power", Activator.myNotify.m_power);
         * representation.setValueInt("brightness", 0);
         * representation.setValueInt("color", 0); response.setErrorCode(200);
         * response.setResourceRepresentation(representation); try {
         * OcPlatform.sendResponse(response); } catch (OcException e) { // TODO
         * Auto-generated catch block e.printStackTrace(); return
         * EntityHandlerResult.ERROR; } } if((requestFlag & 1 << 2) != 0)
         * //ObserverFlag { Log.e(TAG, "requestFlag : Observer"); } } return
         * EntityHandlerResult.OK;
         */

    }

    public void stop(BundleContext bundleContext) throws Exception {
        Activator.AppContext = null;
        Activator.mRichNotificationManager.stop();
    }

    public void perform(int primary) {
        if (primary < 0 || primary >= TemplateTypes.values().length) {
            return;
        }

        Log.w(TAG, "Sending Notification");

        switch (TemplateTypes.values()[primary]) {
            case SMALL_HEADER:
                performExample(new SmallHeaderExample(Activator.AppContext));
                break;
        }
    }

    private void performExample(IExample example) {
        UUID uuid = Activator.mRichNotificationManager.notify(example
                .createRichNoti());
        Log.w(TAG, "Notification ID: " + uuid);
    }
}
