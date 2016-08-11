//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.service.ns.consumer;

import android.util.Log;

public class NSSyncInfo {

    private static final String LOG_TAG = "ConsumerService_NSSyncInfo";

    long messageId              = 0;
    String providerId           = null;
    int state                   = 0;

    public NSSyncInfo(long messageId, String providerId, int state) {

        Log.i (LOG_TAG, "NSSyncInfo()");

        this.messageId = messageId;
        this.providerId = providerId;
        this.state = state;
    }

    public long getMessageId() {
        return messageId;
    }

    public String getProviderId() {
        return providerId;
    }

    public int getState() {
        return state;
    }
}