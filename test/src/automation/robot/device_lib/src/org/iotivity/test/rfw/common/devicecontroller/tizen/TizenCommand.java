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

package org.iotivity.test.rfw.common.devicecontroller.tizen;

public interface TizenCommand {
    final String SDB                       = "sdb";
    final String SHELL                     = "shell";
    final String SPECOPTION                = "-s";
    final String WHITE_SPACE               = " ";
    final String ROOT_ON                   = "root on";
    final String CONNECT                   = "connect";

    final String DLOGUTIL                  = "dlogutil";
    final String CLEAN                     = "-c";

    final String PUSH                      = "push";
    final String PULL                      = "pull";
    final String COPY                      = "cp";
    final String MOVE                      = "mv";
    final String DELETE                    = "rm";
    final String LISTSHOW                  = "ls";
    final String LISTSHOWOPTION            = "-l";
    final String RECURSIVE_FORCE           = "-rf";
    final String MAKE_DIR                  = "mkdir";
    final String CREATE_FILE               = "echo >";
    final String CHANGE_DIR                = "cd";

    final String ROOT                      = "su";
    final String WIFI_SERVICE_ENABLE       = "/opt/usr/apps/org.tizen.phoneinfo/bin/phoneinfo 1";
    final String WIFI_SERVICE_DISABLE      = "/opt/usr/apps/org.tizen.phoneinfo/bin/phoneinfo 2";
    final String BLUETOOTH_SERVICE_ENABLE  = "/opt/usr/apps/org.tizen.phoneinfo/bin/phoneinfo 4";
    final String BLUETOOTH_SERVICE_DISABLE = "/opt/usr/apps/org.tizen.phoneinfo/bin/phoneinfo 6";
    final String GET_IP_ADDRESS            = "/opt/usr/apps/org.tizen.phoneinfo/bin/phoneinfo 3";
    final String GET_MAC_ADDRESS           = "/opt/usr/apps/org.tizen.phoneinfo/bin/phoneinfo 5";

    final String PYTHON_PATH               = "/var/python/bin/python2.7";
    final String SCREEN_CONTROLLER         = "/var/python/bin/ScreenCommand.py";
    final String SEND_KEY                  = "/var/python/bin/fmbttizenagent_cheese";
    final String SEND_HW_KEY_CMD           = "sendKey";

    final String INSTALL_APP               = "install";
    final String StartADBServer            = "start-server";
    final String KillADBServer             = "kill-server";
    // private final String SDBActivatePermission = "root on";
    final String RebootEvent               = "shell reboot";
    final String StartService              = "shell launch_app";
    final String StopService               = "shell pkgcmd -k -n";
}
