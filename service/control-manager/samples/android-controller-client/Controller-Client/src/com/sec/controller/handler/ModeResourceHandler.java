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

import gen.client.resource.IModeResourceResponseListener;
import gen.client.resource.ModeResource;
import gen.cm.MyCMDevice;
import gen.cm.ResourceType;
import gen.xsd.Mode;
import OC.Cm.CMErrorCode;
import OC.Cm.Device;
import OC.Cm.Serialization.Xsd.CMErrorMessage;

import com.sec.controller.CMController;
import com.sec.controller.handler.interfaces.IResourceHandler;
import com.sec.controller.handler.subscription.SubscriptionResponseListener;

public class ModeResourceHandler implements IResourceHandler {

    ModeResource modeResource = null;
    Device       mDevice      = null;

    public ModeResourceHandler(Device device) {

        mDevice = device;
        modeResource = (ModeResource) device.getResource(ResourceType.RT_Mode);
        modeResource.addResponseListener(new ModeResoureceListener(this));
        modeResource.buildPath("0");
    }

    @Override
    public void get() {
        // TODO Auto-generated method stub

        modeResource.getMode();
    }

    @Override
    public void put() {
        // TODO Auto-generated method stub

    }

    @Override
    public void subscribe() {
        // TODO Auto-generated method stub

        try {

            modeResource.subscribe(mDevice.getUUID(),
                    MyCMDevice.getInstance(MyCMDevice.APP_ID).getUUID(),
                    new SubscriptionResponseListener(this), "Mode resrouce");

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

class ModeResoureceListener implements IModeResourceResponseListener {

    ModeResourceHandler mHandler = null;

    public ModeResoureceListener(ModeResourceHandler handler) {

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
        CMController.displayToastMessage("Mode : error " + errorCode);
    }

    @Override
    public boolean onGetMode(int clientSessionId, int statusCode, Mode respData) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Mode : Get Successfull");

            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Mode : Get failed");
        }

        return true;
    }

    @Override
    public boolean onPutMode(int clientSessionId, int statusCode) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Mode : Put Successfull");
            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Mode : Put failed");
        }

        return true;
    }

}
