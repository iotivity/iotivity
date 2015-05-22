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

/// @file HueSharedPreferences.java

package oic.plugin.hue;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

import com.philips.lighting.hue.sdk.connection.impl.PHBridgeInternal;

public class HueSharedPreferences {
    private static final String         HUE_SHARED_PREFERENCES_STORE = "HueSharedPrefs";
    private static final String         LAST_CONNECTED_USERNAME      = "LastConnectedUsername";
    private static final String         LAST_CONNECTED_IP            = "LastConnectedIP";
    private static HueSharedPreferences instance                     = null;
    private SharedPreferences           mSharedPreferences           = null;

    private Editor                      mSharedPreferencesEditor     = null;

    public void create() {

    }

    public static HueSharedPreferences getInstance(Context ctx) {
        if (instance == null) {
            instance = new HueSharedPreferences(ctx);
        }
        return instance;
    }

    private HueSharedPreferences(Context appContext) {
        mSharedPreferences = appContext.getSharedPreferences(
                HUE_SHARED_PREFERENCES_STORE, 0); // 0 - for private mode
        mSharedPreferencesEditor = mSharedPreferences.edit();
    }

    public String getUsername() {
        String username = mSharedPreferences.getString(LAST_CONNECTED_USERNAME,
                "");
        if (username == null || username.equals("")) {
            username = PHBridgeInternal.generateUniqueKey();
            setUsername(username); // Persist the username in the shared prefs
        }
        return username;
    }

    public boolean setUsername(String username) {
        mSharedPreferencesEditor.putString(LAST_CONNECTED_USERNAME, username);
        return (mSharedPreferencesEditor.commit());
    }

    public String getLastConnectedIPAddress() {
        return mSharedPreferences.getString(LAST_CONNECTED_IP, "");
    }

    public boolean setLastConnectedIPAddress(String ipAddress) {
        mSharedPreferencesEditor.putString(LAST_CONNECTED_IP, ipAddress);
        return (mSharedPreferencesEditor.commit());
    }
}