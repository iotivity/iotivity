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

/// @file EntityHandlerHue.java

package oic.plugin.hue;

import java.util.List;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;

import android.util.Log;

import com.philips.lighting.hue.sdk.PHHueSDK;
import com.philips.lighting.model.PHBridge;
import com.philips.lighting.model.PHLight;
import com.philips.lighting.model.PHLightState;

public class EntityHandlerHue implements OcPlatform.EntityHandler {

    final private static String TAG = "EntityHandler";

    public EntityHandlerResult handleEntity(OcResourceRequest resourcerequest) {
        Log.e(TAG, "Entity Handler callback.");

        System.out.println("IN Server CPP entity Handler");
        System.out.println("Hue bulb");
        // OcResourceResponse response = new OcResourceResponse();
        // int result = 1; // EH_ERROR
        if (resourcerequest != null
                && resourcerequest.getResourceUri().equals("/a/huebulb")) {
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
                        PHBridge bridge = PHHueSDK.getInstance()
                                .getSelectedBridge();
                        List<PHLight> myLights = bridge.getResourceCache()
                                .getAllLights();
                        for (PHLight light : myLights) {
                            PHLightState lightState = new PHLightState();
                            lightState = light.getLastKnownLightState();
                            if (lightState.isOn()) {
                                Activator.myLight.m_power = "on";
                            } else if (!lightState.isOn()) {
                                Activator.myLight.m_power = "off";
                            }
                            Activator.myLight.m_color = lightState.getHue();
                            Activator.myLight.m_brightness = lightState
                                    .getBrightness();
                        }
                        break;
                    case PUT:
                        PHBridge mbridge = PHHueSDK.getInstance()
                                .getSelectedBridge();
                        List<PHLight> mmyLights = mbridge
                                .getResourceCache().getAllLights();
                        for (PHLight light : mmyLights) {
                            PHLightState lightState = new PHLightState();
                            String str = resourcerequest
                                    .getResourceRepresentation()
                                    .getValueString("power");
                            if (str.equals("on")) {
                                lightState.setOn(true);
                                Activator.myLight.m_power = "on";
                            } else if (str.equals("off")) {
                                lightState.setOn(false);
                                Activator.myLight.m_power = "off";
                            }
                            lightState.setHue(resourcerequest
                                    .getResourceRepresentation()
                                    .getValueInt("color"));
                            Activator.myLight.m_color = resourcerequest
                                    .getResourceRepresentation()
                                    .getValueInt("color");
                            mbridge.updateLightState(light, lightState);
                        }
                        break;
                    case POST:
                        break;
                }
                response.setErrorCode(200);
                // representation.setUri("/a/huebulb");
                representation.setValueString("name",
                        Activator.myLight.m_name);
                representation.setValueString("power",
                        Activator.myLight.m_power);
                representation.setValueInt("brightness",
                        Activator.myLight.m_brightness);
                representation.setValueInt("color",
                        Activator.myLight.m_color);
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
