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
package oic.ctt.ui.log;

import org.slf4j.Logger;

import oic.ctt.logmanager.LogManager;
import oic.ctt.ui.util.CTLogger;
import oic.ctt.ui.views.LogView;

import com.android.ddmlib.Log.LogLevel;

public class LogHandler {
    private static Logger       logger                  = CTLogger
                                                                .getInstance();
    private static final String LOG_LEVEL_WARN          = "WARN";
    private static final String LOG_LEVEL_ERROR         = "ERROR";
    private static final String LOG_LEVEL_TRACE         = "TRACE";
    private static final String LOG_LEVEL_DEBUG         = "DEBUG";
    private static final String LOG_LEVEL_INFO          = "INFO";
    private static final String WHITESPACE_STRING       = " ";
    private static final String SPLIT_TEXT              = "#s#";
    private static final String SEPERATOR_ROBOT_SYS_LOG = "RobotSysLog";
    private static final String SEPERATOR_ROBOT_LOG     = "RobotLog";
    private static final String SEPERATOR_ROBOT         = "Robot";
    private static final String EMPTY_STRING            = "";

    private static int          LOGVIEWPORT             = 3500;
    private static Thread       threadLogConsumer;
    private static LogView      mLogView;
    public static Boolean       stopRealtimeLogging     = false;
    public static Boolean       stopLogView             = false;

    // start point 1
    public static void startLogViewer(LogView logView) {
        mLogView = logView;
        startLogViewConsumer();
        stopLogView = false;
    }

    public static boolean checkHaveAddress(String[] allElements) {
        boolean rtn = false;
        int elementsLength = allElements.length;
        for (int i = 0; i < elementsLength; i++) {
            if (allElements[i] != null && !allElements[i].isEmpty()
                    && allElements[i] != EMPTY_STRING
                    && allElements[i] != WHITESPACE_STRING) {
                rtn = true;
            }
        }
        return rtn;
    }

    public static String[] removeNullElements(String[] allElements) {
        int n = 0;
        int elementsLength = allElements.length;
        for (int i = 0; i < elementsLength; i++) {
            if (allElements[i] != null && !allElements[i].isEmpty()
                    && allElements[i] != EMPTY_STRING
                    && allElements[i] != WHITESPACE_STRING) {
                n++;
            }
        }
        String[] localAllElements = new String[n];

        int j = 0;
        for (int i = 0; i < elementsLength; i++) {
            if (allElements[i] != null && !allElements[i].isEmpty()
                    && allElements[i] != EMPTY_STRING
                    && allElements[i] != WHITESPACE_STRING) {
                localAllElements[j++] = allElements[i];
            }
        }
        return localAllElements;
    }

    public static void stopLogViewer() {
        stopLogView = true;
        logger.info("stop stopLogViewer");
        LogManager.stopLogManager();
        try {
            if (LogViewConsumer.serverSocketLog != null) {
                if (LogViewConsumer.serverSocketLog.isBound()) {
                    LogViewConsumer.serverSocketLog.close();
                }
            }
            if (LogViewConsumer.socketLog != null) {
                if (LogViewConsumer.socketLog.isBound()) {
                    LogViewConsumer.socketLog.close();
                }
            }
            if (threadLogConsumer != null) {
                if (threadLogConsumer.isAlive()) {
                    threadLogConsumer.sleep(3000);
                    threadLogConsumer.stop();
                }
            }
        } catch (Exception e) {
            logger.info("stopLogView error :" + e.toString());
        }
    }

    public static void stopRealTimeLogging() {
        try {
            stopRealtimeLogging = true;
            if (RealTimeLogViewConsumer.socketLog != null) {
                for (int i = 0; i < RealTimeLogViewConsumer.serverSocketLog.length; i++) {
                    if (RealTimeLogViewConsumer.serverSocketLog[i] != null) {
                        RealTimeLogViewConsumer.serverSocketLog[i].close();
                        logger.info("UILiveLogReceiver socket close : " + i);
                    }
                }
                LogManager.stopLogManager();
                logger.info("stop stopLiveLogging");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void startLogViewConsumer() {
        threadLogConsumer = new Thread(new LogViewConsumer(LOGVIEWPORT,
                mLogView));
        threadLogConsumer.start();
        logger.info("thread run startLogViewConsumer");
    }

    public static void startLogManager() {
        Thread threadLogManager = new Thread() {
            public void run() {
                LogManager.startLogManager(LOGVIEWPORT);
            }
        };
        threadLogManager.start();
        logger.info("thread run startLogManager");
    }

    public static LogLevel convertLogLevel(String response) {
        LogLevel level = null;
        level = LogLevel.INFO;

        if (response != null) {
            if (response.contains(LOG_LEVEL_INFO)) {
                level = LogLevel.INFO;
            } else if (response.contains(LOG_LEVEL_DEBUG)) {
                level = LogLevel.DEBUG;
            } else if (response.contains(LOG_LEVEL_TRACE)) {
                level = LogLevel.INFO;
            } else if (response.contains(LOG_LEVEL_ERROR)) {
                level = LogLevel.ERROR;
            } else if (response.contains(LOG_LEVEL_WARN)) {
                level = LogLevel.WARN;
            }
        }

        return level;
    }

    /*
     * 0 1 2 3 4 20150915 13:29:19.082 #s# TRACE #s# RobotLog #s# ZWave #s#
     * Arguments: [ '15s' ] 0 1 2 3 20150915 13:29:19.082 #s# TRACE #s# RobotLog
     * #s##s# Arguments: [ '15s' ] 20150915 13:29:19.082 #s# TRACE #s##s# ZWave
     * #s# Arguments: [ '15s' ] 0 1 2 20150915 13:29:19.082 #s# TRACE #s##s##s#
     * Arguments: [ '15s' ] #s##s##s# Arguments: [ '15s' ]
     */
    public static String[] convertLogMessage(String response) {
        String[] splitResponse = null;
        // System.out.println("before convert log : " + response);
        // 20150922 17:57:40.593 #s# TRACE #s# RobotLog #s# Return: None
        // level, time, msg, tag, syslog
        String[] arrReturn = { EMPTY_STRING, EMPTY_STRING, WHITESPACE_STRING,
                EMPTY_STRING, EMPTY_STRING };
        if (response != null) {
            if (response.contains(SPLIT_TEXT)) {
                splitResponse = response.split(SPLIT_TEXT);

                int responseLength = splitResponse.length;
                if (splitResponse[0].startsWith("20")) {
                    arrReturn[1] = splitResponse[0];

                    if (responseLength == 6) {
                        arrReturn[0] = splitResponse[1];
                        arrReturn[2] = "[" + splitResponse[4] + "]"
                                + splitResponse[5];
                        arrReturn[3] = splitResponse[3];
                        arrReturn[4] = splitResponse[2];
                    } else if (responseLength == 5) {
                        arrReturn[0] = splitResponse[1];
                        arrReturn[2] = splitResponse[4];
                        arrReturn[3] = splitResponse[3];
                        arrReturn[4] = splitResponse[2];
                    } else if (responseLength == 4) {
                        arrReturn[0] = splitResponse[1];
                        arrReturn[2] = splitResponse[3];
                        if (splitResponse[2].startsWith(SEPERATOR_ROBOT)) {
                            arrReturn[4] = splitResponse[2];
                        } else {
                            arrReturn[3] = splitResponse[2];
                            arrReturn[4] = SEPERATOR_ROBOT_LOG;
                        }
                    } else if (responseLength == 3) {
                        if (splitResponse[1].startsWith(SEPERATOR_ROBOT)) {
                            arrReturn[4] = splitResponse[1];
                            arrReturn[0] = LOG_LEVEL_INFO;
                        } else {
                            arrReturn[0] = splitResponse[1];
                            arrReturn[4] = SEPERATOR_ROBOT_SYS_LOG;
                        }

                        arrReturn[2] = splitResponse[2];
                        arrReturn[3] = EMPTY_STRING;

                    } else if (responseLength == 2) {
                        arrReturn[2] = splitResponse[1];
                        arrReturn[0] = splitResponse[0];
                        arrReturn[4] = SEPERATOR_ROBOT_SYS_LOG;
                    }
                } else {
                    if (responseLength == 2) {
                        arrReturn[2] = splitResponse[1];
                        if (splitResponse[0].startsWith(SEPERATOR_ROBOT)) {
                            arrReturn[4] = splitResponse[0];
                            arrReturn[0] = LOG_LEVEL_INFO;
                        } else {
                            arrReturn[4] = SEPERATOR_ROBOT_SYS_LOG;
                            arrReturn[0] = splitResponse[0];
                        }

                    } else {
                        arrReturn[2] = response;
                        arrReturn[0] = LOG_LEVEL_INFO;
                        arrReturn[4] = SEPERATOR_ROBOT_SYS_LOG;
                    }
                }
            } else {
                arrReturn[2] = response;
                arrReturn[0] = LOG_LEVEL_INFO;
                arrReturn[4] = SEPERATOR_ROBOT_SYS_LOG;
            }
        }
        return arrReturn;
    }
}