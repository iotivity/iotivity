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

import gen.client.resource.ITemperaturesResourceResponseListener;
import gen.client.resource.TemperaturesResource;
import gen.cm.MyCMDevice;
import gen.cm.ResourceType;
import gen.xsd.Temperatures;
import OC.Cm.CMErrorCode;
import OC.Cm.Device;
import OC.Cm.Serialization.Xsd.CMErrorMessage;

import com.sec.controller.CMController;
import com.sec.controller.handler.interfaces.IResourceHandler;
import com.sec.controller.handler.subscription.SubscriptionResponseListener;

public class TemperatureResourceHandler implements IResourceHandler {

    TemperaturesResource temperatureResource = null;
    Device               mDevice             = null;

    public TemperatureResourceHandler(Device device) {

        mDevice = device;
        temperatureResource = (TemperaturesResource) device
                .getResource(ResourceType.RT_Temperatures);
        temperatureResource
                .addResponseListener(new TemperaturesResoureceListener(this));
        temperatureResource.buildPath("0");
    }

    @Override
    public void get() {
        // TODO Auto-generated method stub

        temperatureResource.getTemperatures();
    }

    @Override
    public void put() {
        // TODO Auto-generated method stub

    }

    @Override
    public void subscribe() {
        // TODO Auto-generated method stub

        try {

            temperatureResource.subscribe(mDevice.getUUID(), MyCMDevice
                    .getInstance(MyCMDevice.APP_ID).getUUID(),
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

class TemperaturesResoureceListener implements
        ITemperaturesResourceResponseListener {

    TemperatureResourceHandler mHandler = null;

    public TemperaturesResoureceListener(TemperatureResourceHandler handler) {

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
        CMController.displayToastMessage("Temperature : error " + errorCode);
    }

    @Override
    public boolean onGetTemperatures(int clientSessionId, int statusCode,
            Temperatures respData) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Temperatures : Get Successfull");

            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Temperatures : Get failed");
        }

        return true;
    }

    @Override
    public boolean onPutTemperatures(int clientSessionId, int statusCode) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Temperatures : Put Successfull");
            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Temperatures : Put failed");
        }

        return true;
    }

}
