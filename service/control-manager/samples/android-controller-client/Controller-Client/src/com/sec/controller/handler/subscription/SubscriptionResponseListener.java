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

package com.sec.controller.handler.subscription;

import OC.Cm.CMErrorCode;
import OC.Cm.Client.Resource.IResourceResponseListener;
import OC.Cm.Connector.HeadersList;
import OC.Cm.Serialization.ISerializable;
import android.util.Log;

import com.sec.controller.handler.interfaces.IResourceHandler;

public class SubscriptionResponseListener implements IResourceResponseListener {

    IResourceHandler mHandler = null;

    public SubscriptionResponseListener(IResourceHandler handler) {

        mHandler = handler;
    }

    @Override
    public void OnGetResponseReceived(int clientSessionId, int status,
            HeadersList headers, ISerializable response) {
        // TODO Auto-generated method stub

        Log.i("SubscriptionResponseListener", "OnGetResponseReceived ");

    }

    @Override
    public void OnPutResponseReceived(int clientSessionId, int status,
            HeadersList headers, ISerializable response) {
        // TODO Auto-generated method stub
        Log.i("SubscriptionResponseListener", "OnPutResponseReceived ");
    }

    @Override
    public void OnPostResponseReceived(int clientSessionId, int status,
            HeadersList headers, ISerializable response) {
        // TODO Auto-generated method stub
        Log.i("SubscriptionResponseListener", "OnPostResponseReceived ");
    }

    @Override
    public void onDeleteResponseReceived(int clientSessionId, int status,
            HeadersList headers, ISerializable response) {
        // TODO Auto-generated method stub
        Log.i("SubscriptionResponseListener", "onDeleteResponseReceived ");
    }

    @Override
    public void onSubscribeResponseReceived(int clientSessionId, int status,
            String subscriptionUri) {
        // TODO Auto-generated method stub

        Log.i("SubscriptionResponseListener",
                " onSubscribeResponseReceived subscriptionUri = "
                        + subscriptionUri);

    }

    @Override
    public void onUnSubscribeResponseReceived(int clientSessionId, int status) {
        // TODO Auto-generated method stub
        Log.i("SubscriptionResponseListener", "onUnSubscribeResponseReceived ");
    }

    @Override
    public void onErrorReceived(int clientSessionId, int status,
            CMErrorCode error_code, String errorMessage, ISerializable error) {
        // TODO Auto-generated method stub
        Log.i("SubscriptionResponseListener", "onErrorReceived ");
    }

    @Override
    public void onRequestComplete(int clientSessionId, String requestPayload,
            String responsePayload) {
        // TODO Auto-generated method stub
        Log.i("SubscriptionResponseListener", "onRequestComplete ");
    }

}
