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

public class NSMessage {

    private static final String LOG_TAG = "ConsumerService_NSMessage";

    long messageId                     = 0;
    String providerId                  = null;

    String title                       = null;
    String contentText                 = null;
    String sourceName                  = null;

    private long mNativeHandle         = 0;

    public NSMessage(long messageId, String providerId, String title,
        String contentText, String sourceName, long mNativeHandle) {

        Log.i (LOG_TAG, "NSMessage()");

        this.messageId = messageId;
        this.providerId = providerId;
        this.title = title;
        this.contentText = contentText;
        this.sourceName = sourceName;

        this.mNativeHandle = mNativeHandle;
    }

    public long getMessageId() {
        return messageId;
    }

    public String getProviderId() {
        return providerId;
    }

    public String getTitle() {
        return title;
    }

    public String getContentText() {
        return contentText;
    }

    public String getSource() {
        return sourceName;
    }
}