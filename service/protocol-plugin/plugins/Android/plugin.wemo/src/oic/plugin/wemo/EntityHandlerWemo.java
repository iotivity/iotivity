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

/// @file EntityHandlerWemo.java

package oic.plugin.wemo;

import java.util.ArrayList;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;

import android.util.Log;

import com.belkin.wemo.localsdk.WeMoDevice;

public class EntityHandlerWemo implements OcPlatform.EntityHandler {

    final private static String TAG = "EntityHandlerWeMo";

    public EntityHandlerResult handleEntity(OcResourceRequest resourcerequest) {
        Log.e(TAG, "Entity Handler callback");

        // OcResourceResponse response = new OcResourceResponse();

        if (resourcerequest != null
                && resourcerequest.getResourceUri().equals("/a/wemo")) {
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
                        ArrayList<String> udns = Activator.mWeMoSDKContext
                                .getListOfWeMoDevicesOnLAN();
                        for (String udn : udns) {
                            WeMoDevice wemoDevice = Activator.mWeMoSDKContext
                                    .getWeMoDeviceByUDN(udn);
                            if (wemoDevice.getState() == "0") {
                                Activator.mySmartPlug.m_power = "off";
                            } else if (wemoDevice.getState() == "1") {
                                Activator.mySmartPlug.m_power = "on";
                            }
                        }
                        break;
                    case PUT:
                        ArrayList<String> uudns = Activator.mWeMoSDKContext
                                .getListOfWeMoDevicesOnLAN();
                        for (String udn : uudns) {
                            WeMoDevice wemoDevice = Activator.mWeMoSDKContext
                                    .getWeMoDeviceByUDN(udn);
                            String type = wemoDevice.getType();
                            if (type.equals(WeMoDevice.SWITCH)) {
                                String newState = "";
                                if (resourcerequest
                                        .getResourceRepresentation()
                                        .getValueString("power")
                                        .equals("on")) {
                                    Activator.mySmartPlug.m_power = "on";
                                    newState = WeMoDevice.WEMO_DEVICE_ON;
                                } else if (resourcerequest
                                        .getResourceRepresentation()
                                        .getValueString("power")
                                        .equals("off")) {
                                    Activator.mySmartPlug.m_power = "off";
                                    newState = WeMoDevice.WEMO_DEVICE_OFF;
                                }
                                Activator.mWeMoSDKContext.setDeviceState(
                                        newState, wemoDevice.getUDN());
                            }
                        }
                        break;
                    case POST:
                        break;
                }
                response.setErrorCode(200);
                // representation.setUri("/a/wemo");
                representation.setValueString("name",
                        Activator.mySmartPlug.m_name);
                representation.setValueString("power",
                        Activator.mySmartPlug.m_power);
                representation.setValueInt("brightness", 0);
                representation.setValueInt("color", 0);
                response.setResourceRepresentation(representation);
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
    }
}
