/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
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
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.android;

public interface AndroidCommand {

    static final String ADB                       = "adb";
    static final String SHELL                     = "shell";
    static final String CONNECT                   = "connect";

    final String        PUSH                      = "push";
    final String        PULL                      = "push";
    final String        COPY                      = "cp";
    final String        MOVE                      = "mv";
    final String        DELETE                    = "rm";
    final String        LISTSHOW                  = "ls";
    final String        LISTSHOWOPTION            = "-l";
    final String        SPECOPTION                = "-s";
    final String        LOGCAT                    = "logcat";
    final String        CLEAN                     = "-c";
    final String        WHITE_SPACE               = " ";
    final String        ROOT                      = "su";
    final String        PROCESS                   = "ps";
    final String        RECURSIVE_FORCE           = "-rf";
    final String        CREATE_FILE               = "touch";
    final String        CHANGE_DIR                = "cd";
    final String        MAKE_DIR                  = "mkdir";

    static final String START_ADB_SERVER          = "start-server";
    static final String KILL_ADB_SERVER           = "kill-server";
    final String        WIFI_SERVICE_ENABLE       = "svc wifi enable";
    final String        WIFI_SERVICE_DISABLE      = "svc wifi disable";
    final String        BLUETOOTH_SERVICE_ENABLE  = "adb shell am start -a android.bluetooth.adapter.action.REQUEST_ENABLE";
    final String        BLUETOOTH_SERVICE_DISABLE = "service call bluetooth_manager 10";
    final String        GET_IP_CMD                = "adb shell ip add";
    final String        GET_MAC_ADDRESS           = "adb shell settings get secure bluetooth_address";

    final String        LOCAL_IP_ADDRESS          = "127.0.0.1";

}
