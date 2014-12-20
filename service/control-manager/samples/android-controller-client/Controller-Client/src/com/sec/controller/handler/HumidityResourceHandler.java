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

import gen.client.resource.HumidityResource;
import gen.client.resource.IHumidityResourceResponseListener;
import gen.cm.MyCMDevice;
import gen.cm.ResourceType;
import gen.xsd.Humidity;
import OC.Cm.CMErrorCode;
import OC.Cm.Device;
import OC.Cm.Serialization.Xsd.CMErrorMessage;

import com.sec.controller.CMController;
import com.sec.controller.handler.interfaces.IResourceHandler;
import com.sec.controller.handler.subscription.SubscriptionResponseListener;

public class HumidityResourceHandler implements IResourceHandler {

    HumidityResource humidityResource = null;
    Device           mDevice          = null;

    public HumidityResourceHandler(Device device) {

        mDevice = device;
        humidityResource = (HumidityResource) device
                .getResource(ResourceType.RT_Humidity);
        humidityResource
                .addResponseListener(new HumidityResoureceListener(this));
        humidityResource.buildPath("0");
    }

    @Override
    public void get() {
        // TODO Auto-generated method stub

        humidityResource.getHumidity();
    }

    @Override
    public void put() {
        // TODO Auto-generated method stub

    }

    @Override
    public void subscribe() {
        // TODO Auto-generated method stub

        try {

            humidityResource
                    .subscribe(
                            mDevice.getUUID(),
                            MyCMDevice.getInstance(MyCMDevice.APP_ID).getUUID(),
                            new SubscriptionResponseListener(this),
                            "Humidity resrouce");

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

class HumidityResoureceListener implements IHumidityResourceResponseListener {

    HumidityResourceHandler mHandler = null;

    public HumidityResoureceListener(HumidityResourceHandler handler) {

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
        CMController.displayToastMessage("Humidity : error " + errorCode);
    }

    @Override
    public boolean onGetHumidity(int clientSessionId, int statusCode,
            Humidity respData) {
        if (statusCode == 200) {
            CMController.displayToastMessage("Humidity : Get Successfull");
            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Humidity : Get failed");
        }

        return true;
    }

    @Override
    public boolean onPutHumidity(int clientSessionId, int statusCode) {
        // TODO Auto-generated method stub
        return false;
    }

}
