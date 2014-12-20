//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

package com.sec.controller.handler;

import gen.client.resource.ILightResourceResponseListener;
import gen.client.resource.LightResource;
import gen.cm.MyCMDevice;
import gen.cm.ResourceType;
import gen.xsd.Light;
import OC.Cm.CMErrorCode;
import OC.Cm.Device;
import OC.Cm.Serialization.Xsd.CMErrorMessage;

import com.sec.controller.CMController;
import com.sec.controller.handler.interfaces.IResourceHandler;
import com.sec.controller.handler.subscription.SubscriptionResponseListener;

public class LightResourceHandler implements IResourceHandler {

    LightResource lightResource = null;
    Device        mDevice       = null;

    public LightResourceHandler(Device device) {

        mDevice = device;
        lightResource = (LightResource) device
                .getResource(ResourceType.RT_Light);
        lightResource.addResponseListener(new LightResoureceListener(this));
        lightResource.buildPath("0");
    }

    @Override
    public void get() {
        // TODO Auto-generated method stub

        lightResource.getLight();
    }

    @Override
    public void put() {
        // TODO Auto-generated method stub

    }

    @Override
    public void subscribe() {
        // TODO Auto-generated method stub

        try {

            lightResource.subscribe(mDevice.getUUID(),
                    MyCMDevice.getInstance(MyCMDevice.APP_ID).getUUID(),
                    new SubscriptionResponseListener(this), "Light resrouce");

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public void unsubscribe() {
        // TODO Auto-generated method stub

    }

}

class LightResoureceListener implements ILightResourceResponseListener {

    LightResourceHandler mHandler = null;

    public LightResoureceListener(LightResourceHandler handler) {

        mHandler = handler;
    }

    @Override
    public void onSubscribeResponseReceived(int clientSessionId,
            int statusCode, String subscriptionUri) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onUnSubscribeResponseReceived(int clientSessionId,
            int statusCode) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onError(int clientSessionId, int statusCode,
            CMErrorCode errorCode, CMErrorMessage error) {
        // TODO Auto-generated method stub
        CMController.displayToastMessage("Light : error " + errorCode);
    }

    @Override
    public boolean onGetLight(int clientSessionId, int statusCode,
            Light respData) {
        // TODO Auto-generated method stub

        if (statusCode == 200) {
            CMController.displayToastMessage("Light : Get Successfull");

            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Light : Get failed");
        }

        return true;
    }

    @Override
    public boolean onPutLight(int clientSessionId, int statusCode) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Light : Put Successfull");
            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Light : Put failed");
        }

        return true;
    }

}
