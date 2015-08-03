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

/// @file PluginManager.java

/// @brief After installing a plug-in in a directory, each plug-in can be managed by this class.

package org.iotivity.service.ppm;

import android.content.Context;
import android.util.Log;

public class PluginManager {

    /**
    * Start plug-ins by key-value pair.
    *
    * @param key    - key information of the plug-in to be started.
    * @param value   - value information of the plug-in to be started.
    * @return int   - 1 on success or 0 on failure.
    */
    public int startPlugins(String key, String value) {
        LogEx("startPlugins() Called.");
        return jniStartPlugins(key, value);
    }

    /**
    * Stop plug-ins by key-value pair.
    *
    * @param key    - key information of the plug-in to be stopped.
    * @param value   - value information of the plug-in to be stopped.
    * @return int   - 1 on success or 0 on failure.
    */
    public int stopPlugins(String key, String value) {
        LogEx("stopPlugins() Called.");
        return jniStopPlugins(key, value);
    }

    /**
    * Rescan for plug-ins in the registered directory and install plug-ins.
    *
    * @param void
    * @return int   - 1 on success or 0 on failure.
    */
    public int rescanPlugin() {
        LogEx("rescanPlugin() Called.");
        return jniRescanPlugin();
    }

    /**
    * Get list of plug-ins.
    *
    * @param void
    * @return Plugin[]   - Plug-in array.
    */
    public Plugin[] getPlugins() {
        LogEx("getPlugins() Called.");
        return jniGetPlugins();
    }

    /**
    * Get state of the plug-in.
    *
    * @param pluginID       - ID of the plug-in to get state.
    * @return String   - state of the plug-in.
    */
    public String getState(String plugID) {
        LogEx("getState() Called.");
        return jniGetState(plugID);
    }

    public PluginManager(Context ctx) {
        LogEx("PluginManager() Called.");
        FelixManager.getInstance(ctx);
        System.loadLibrary("PluginManager");
    }

    private static final String LOG_TAG = "PPMSampleApp : PluginManager";

    private native int jniStartPlugins(String key, String value);

    private native int jniStopPlugins(String key, String value);

    private native int jniRescanPlugin();

    private native Plugin[] jniGetPlugins();

    private native String jniGetState(String plugID);

    private static void LogEx(String info) {
        Log.d(LOG_TAG, info);
    }
}
