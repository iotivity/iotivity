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

/// @file SmallHeaderExample.java

package oic.plugin.gear.noti;

import android.content.Context;

import com.samsung.android.sdk.richnotification.SrnRichNotification;
import com.samsung.android.sdk.richnotification.SrnRichNotification.AlertType;
import com.samsung.android.sdk.richnotification.templates.SrnPrimaryTemplate;
import com.samsung.android.sdk.richnotification.templates.SrnStandardTemplate;
import com.samsung.android.sdk.richnotification.templates.SrnStandardTemplate.HeaderSizeType;

public class SmallHeaderExample implements IExample {

    private final Context mContext;

    public SmallHeaderExample(Context ctx) {
        mContext = ctx;
    }

    @Override
    public SrnRichNotification createRichNoti() {
        SrnRichNotification noti = new SrnRichNotification(mContext);
        noti.setReadout(EntityHandlerNoti.textNoti, "");
        noti.setPrimaryTemplate(getSmallHeaderTemplate());

        noti.setAlertType(AlertType.VIBRATION);

        return noti;
    }

    public SrnPrimaryTemplate getSmallHeaderTemplate() {
        SrnStandardTemplate smallHeaderTemplate = new SrnStandardTemplate(
                HeaderSizeType.SMALL);

        smallHeaderTemplate.setSubHeader(EntityHandlerNoti.textNoti);

        return smallHeaderTemplate;
    }
}
