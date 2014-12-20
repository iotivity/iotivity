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

import gen.client.resource.INetworkResourceResponseListener;
import gen.client.resource.NetworkResource;
import gen.cm.MyCMDevice;
import gen.cm.ResourceType;
import gen.xsd.Network;
import OC.Cm.CMErrorCode;
import OC.Cm.Device;
import OC.Cm.Serialization.Xsd.CMErrorMessage;

import com.sec.controller.CMController;
import com.sec.controller.handler.interfaces.IResourceHandler;
import com.sec.controller.handler.subscription.SubscriptionResponseListener;

public class NetworkResourceHandler implements IResourceHandler {

    NetworkResource networkResource = null;
    Device          mDevice         = null;

    public NetworkResourceHandler(Device device) {

        mDevice = device;
        networkResource = (NetworkResource) device
                .getResource(ResourceType.RT_Network);
        networkResource.addResponseListener(new NetworkResoureceListener(this));
        networkResource.buildPath("0", "0");
    }

    @Override
    public void get() {
        // TODO Auto-generated method stub

        networkResource.getNetwork();
    }

    @Override
    public void put() {
        // TODO Auto-generated method stub

        // networkResource.putNetwork(reqData);
    }

    @Override
    public void subscribe() {
        // TODO Auto-generated method stub

        try {

            networkResource.subscribe(mDevice.getUUID(), MyCMDevice
                    .getInstance(MyCMDevice.APP_ID).getUUID(),
                    new SubscriptionResponseListener(this), "Network resrouce");

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

class NetworkResoureceListener implements INetworkResourceResponseListener {

    NetworkResourceHandler mHandler = null;

    public NetworkResoureceListener(NetworkResourceHandler handler) {

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

        CMController.displayToastMessage("Network : error " + errorCode);
    }

    @Override
    public boolean onGetNetwork(int clientSessionId, int statusCode,
            Network respData) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Network : Get Successfull");

            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Network : Get failed");
        }

        return true;
    }

    @Override
    public boolean onPutNetwork(int clientSessionId, int statusCode) {
        // TODO Auto-generated method stub
        if (statusCode == 200) {
            CMController.displayToastMessage("Network : Put Successfull");
            mHandler.subscribe();
        } else {
            CMController.displayToastMessage("Network : Put failed");
        }

        return true;
    }

}
