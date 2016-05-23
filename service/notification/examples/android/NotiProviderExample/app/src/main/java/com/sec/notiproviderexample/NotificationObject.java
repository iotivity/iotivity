/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package com.sec.notiproviderexample;

public class NotificationObject {

    private static final String TITLE_KEY = "title";
    private static final String ID_KEY = "id";
    private static final String CONTENT_TEXT_KEY = "contentText";
    private static final String TAG = "NS_JNI_NOTI_OBJECT";

    private String mId = null;
    private String mTitle = null;
    private String mContentText = null;

    public NotificationObject(String id, String title) {
        this.mId = id;
        this.mTitle = title;
    }

    public String getContentText() {

        return mContentText;
    }
    public String getmTitle() {

        return mTitle;
    }
    public String getmId() {

        return mId;
    }

    public void setmId(String mId) {

        this.mId = mId;
    }

    public void setContentText(String contentText) {

        this.mContentText = contentText;
    }
}
