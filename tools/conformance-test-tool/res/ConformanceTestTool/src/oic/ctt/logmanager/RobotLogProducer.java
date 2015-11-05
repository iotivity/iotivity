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
package oic.ctt.logmanager;

public class RobotLogProducer {

    private static String      sendingLog = null;
    public static final String SPLIT      = "#s#";

    public RobotLogProducer() {
    }

    public static void info_log(String tag, String msg) {
        if (msg == null) {
            msg = "No message";
        }

        sendingLog = "*INFO* " + LogCollector.setNomalizeTag(tag) + "" + msg;
        System.out.println(sendingLog);
    }

    public static void debug_log(String tag, String msg) {
        if (msg == null) {
            msg = "No message";
        }

        sendingLog = "*DEBUG* " + LogCollector.setNomalizeTag(tag) + "" + msg;
        System.out.println(sendingLog);
    }

    public static void warn_log(String tag, String msg) {
        if (msg == null) {
            msg = "No message";
        }

        sendingLog = "*WARN* " + LogCollector.setNomalizeTag(tag) + "" + msg;
        System.out.println(sendingLog);
    }

    public static void trace_log(String tag, String msg) {
        if (msg == null) {
            msg = "No message";
        }

        sendingLog = "*TRACE* " + LogCollector.setNomalizeTag(tag) + "" + msg;
        System.out.println(sendingLog);
    }

    public static void error_log(String tag, String msg) {
        if (msg == null) {
            msg = "No message";
        }

        sendingLog = "*ERROR* " + LogCollector.setNomalizeTag(tag) + "" + msg;
        System.out.println(sendingLog);
    }
}
