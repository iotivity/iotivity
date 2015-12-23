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
package oic.ctt.ui.actions;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;

public class ActionsConstants {

    private static final int   BIT                    = 1024;
    public static final String COMMAND_TYPE_SHELL     = "/bin/sh";
    public static final String COMMAND_GREP           = "pgrep -P ";
    public static final String COMMAND_KILL           = "kill ";
    public static final String COMMAND_PROCESS        = "ps -o command -p";
    public static final String UNICODE_TYPE           = "UTF-8";
    public static final String C                      = "-c";
    public static final String PKILL_19_P             = "pkill -19 -P ";
    public static final String PKILL_18_P             = "pkill -18 -P ";
    public static final String KILL_19                = "kill -19 ";
    public static final String KILL_18                = "kill -18 ";
    public static final String COMMAND_PROCESS_SEARCH = "ps ";
    public static final String JYTHON_JAR_FILE_NAME   = "jython.jar";
    public static final String AWK_END_PRINT          = " | awk END'{print}'";
    public static final String AWK_END_PRINT_$3       = "| awk END'{print $3}'";
    public static final String MENU_ITEM_RUN_TEXT     = "Run";

    /**
     * this method executes shell command
     * 
     * @param command
     *            array
     * @return process
     * @throws IOException
     */
    public static Process executeCommand(String[] cmd) throws IOException {
        return Runtime.getRuntime().exec(cmd);
    }

    /**
     * used to get process id
     * 
     * @param pidKillProcess
     *            Process object
     * @return process id
     * @throws IOException
     * @throws UnsupportedEncodingException
     */
    public static String getProcessID(Process pidKillProcess)
            throws IOException, UnsupportedEncodingException {
        return new BufferedReader(new InputStreamReader(
                pidKillProcess.getInputStream(), UNICODE_TYPE), BIT).readLine();
    }

}
