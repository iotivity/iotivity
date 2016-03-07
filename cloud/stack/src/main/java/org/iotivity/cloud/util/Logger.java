/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.util;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class Logger {

    public static final int VERBOSE = 0;
    public static final int DEBUG   = 1;
    public static final int INFO    = 2;
    public static final int WARNING = 3;
    public static final int ERROR   = 4;

    private static int logLevel = DEBUG;

    public static void setLogLevel(int level) {
        logLevel = level;
    }

    public static void v(String log) {
        printLog(VERBOSE, log);
    }

    public static void d(String log) {
        printLog(DEBUG, log);
    }

    public static void i(String log) {
        printLog(INFO, log);
    }

    public static void w(String log) {
        printLog(WARNING, log);
    }

    public static void e(String log) {
        printLog(ERROR, log);
    }

    public static void w(String log, Exception ex) {
        printLog(WARNING, log);
        ex.printStackTrace();
    }

    public static void w(String log, Throwable th) {
        printLog(WARNING, log);
        th.printStackTrace();
    }

    public static void e(String log, Exception ex) {
        printLog(ERROR, log);
        ex.printStackTrace();
    }

    public static void e(String log, Throwable th) {
        printLog(ERROR, log);
        th.printStackTrace();
    }

    private static void printLog(int level, String log) {

        if (logLevel > level)
            return;

        String format = "";
        format += getTime();
        format += " [" + getLogLevelString(level) + "]";

        if (level >= ERROR) {
            format += " [" + getDetailInfo() + "]";
        }

        format += " " + log;

        System.out.println(format);
    }

    private static String getDetailInfo() {

        String res = "";

        StackTraceElement ste = Thread.currentThread().getStackTrace()[4];
        String className = ste.getClassName()
                .substring(ste.getClassName().lastIndexOf(".") + 1);

        res += ste.getFileName() + ", " + className + "." + ste.getMethodName()
                + "(), line:" + ste.getLineNumber();

        return res;
    }

    private static String getTime() {
        Calendar calendar = Calendar.getInstance();
        SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss:SSS");

        return dateFormat.format(calendar.getTime());
    }

    private static String getLogLevelString(int level) {

        String res = "";

        if (level == VERBOSE) {
            res = "V";
        } else if (level == DEBUG) {
            res = "D";
        } else if (level == INFO) {
            res = "I";
        } else if (level == WARNING) {
            res = "W";
        } else if (level == ERROR) {
            res = "E";
        }

        return res;
    }
}
