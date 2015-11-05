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

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

import oic.ctt.logmanager.LogManager;
import oic.ctt.ui.views.LogView;

import com.android.ddmlib.Log.LogLevel;

public class LogHandler {
    private static int         LOGVIEWPORT           = 3500;
    private static int         LIVELOGPORT_VALUE     = 4000;
    private static int         LIVELOGPORT           = LIVELOGPORT_VALUE;
    private static Thread      threadLogConsumer;
    private static Thread[]    threadLiveLogConsumer = new Thread[10];
    private static LogView     mLogView;
    public static Boolean      stopRealtimeLogging   = false;
    public static Boolean      stopLogView           = false;
    public static final String SPLIT                 = "#s#";

    // start point 1
    public static void startLogViewer(LogView logView) {
        mLogView = logView;
        startLogViewConsumer();
        stopLogView = false;
    }

    public static boolean checkHaveAddress(String[] allElements) {
        boolean rtn = false;
        for (int i = 0; i < allElements.length; i++) {
            if (allElements[i] != null) {
                if (!allElements[i].isEmpty()) {
                    if (allElements[i] != "") {
                        if (allElements[i] != " ") {
                            rtn = true;
                        }
                    }
                }
            }
        }
        return rtn;
    }

    public static String[] removeNullElements(String[] allElements) {
        int n = 0;
        for (int i = 0; i < allElements.length; i++) {
            if (allElements[i] != null) {
                if (!allElements[i].isEmpty()) {
                    if (allElements[i] != "") {
                        if (allElements[i] != " ") {
                            n++;
                        }
                    }
                }
            }
        }
        String[] localAllElements = new String[n];

        int j = 0;
        for (int i = 0; i < allElements.length; i++) {
            if (allElements[i] != null) {
                if (!allElements[i].isEmpty()) {
                    if (allElements[i] != "") {
                        if (allElements[i] != " ") {
                            localAllElements[j++] = allElements[i];
                        }
                    }
                }
            }
        }
        return localAllElements;
    }

    public static void stopLogViewer() {
        stopLogView = true;
        System.out.println("stop stopLogViewer");
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
            System.out.println("stopLogView error :" + e.toString());
        }
    }

    public static void stopRealTimeLogging() {
        try {
            stopRealtimeLogging = true;
            if (RealTimeLogViewConsumer.socketLog != null) {
                for (int i = 0; i < RealTimeLogViewConsumer.serverSocketLog.length; i++) {
                    if (RealTimeLogViewConsumer.serverSocketLog[i] != null) {
                        RealTimeLogViewConsumer.serverSocketLog[i].close();
                        System.out.println("UILiveLogReceiver socket close : "
                                + i);
                    }
                }
                LogManager.stopLogManager();
                System.out.println("stop stopLiveLogging");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void startLogViewConsumer() {
        threadLogConsumer = new Thread(new LogViewConsumer(LOGVIEWPORT,
                mLogView));
        threadLogConsumer.start();
        System.out.println("thread run startLogViewConsumer");
    }

    public static void startLogManager() {
        Thread threadLogManager = new Thread() {
            public void run() {
                LogManager.startLogManager(LOGVIEWPORT);
            }
        };
        threadLogManager.start();
        System.out.println("thread run startLogManager");
    }

    public static LogLevel convertLogLevel(String response) {
        LogLevel level = null;
        level = LogLevel.INFO;

        if (response != null) {
            if (response.contains("INFO")) {
                level = LogLevel.INFO;
            } else if (response.contains("DEBUG")) {
                level = LogLevel.DEBUG;
            } else if (response.contains("TRACE")) {
                level = LogLevel.INFO;
            } else if (response.contains("ERROR")) {
                level = LogLevel.ERROR;
            } else if (response.contains("WARN")) {
                level = LogLevel.WARN;
            }
        }
        // System.out.println("convert level : " + level.toString());
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
        String[] arrReturn = { "", "", " ", "", "" };
        if (response != null) {
            if (response.contains("#s#")) {
                splitResponse = response.split("#s#");

                if (splitResponse[0].startsWith("20")) {
                    arrReturn[1] = splitResponse[0];

                    if (splitResponse.length == 6) {
                        arrReturn[0] = splitResponse[1];
                        arrReturn[2] = "[" + splitResponse[4] + "]"
                                + splitResponse[5];
                        arrReturn[3] = splitResponse[3];
                        arrReturn[4] = splitResponse[2];
                    } else if (splitResponse.length == 5) {
                        arrReturn[0] = splitResponse[1];
                        arrReturn[2] = splitResponse[4];
                        arrReturn[3] = splitResponse[3];
                        arrReturn[4] = splitResponse[2];
                    } else if (splitResponse.length == 4) {
                        arrReturn[0] = splitResponse[1];
                        arrReturn[2] = splitResponse[3];
                        if (splitResponse[2].startsWith("Robot")) {
                            arrReturn[4] = splitResponse[2];
                        } else {
                            arrReturn[3] = splitResponse[2];
                            arrReturn[4] = "RobotLog";
                        }
                    } else if (splitResponse.length == 3) {

                        if (splitResponse[1].startsWith("Robot")) {
                            arrReturn[4] = splitResponse[1];
                            arrReturn[0] = "INFO";
                        } else {
                            arrReturn[0] = splitResponse[1];
                            arrReturn[4] = "RobotSysLog";
                        }

                        arrReturn[2] = splitResponse[2];
                        arrReturn[3] = "";

                    } else if (splitResponse.length == 2) {
                        arrReturn[2] = splitResponse[1];
                        arrReturn[0] = splitResponse[0];
                        arrReturn[4] = "RobotSysLog";
                    }
                } else {

                    if (splitResponse.length == 2) {
                        arrReturn[2] = splitResponse[1];
                        if (splitResponse[0].startsWith("Robot")) {
                            arrReturn[4] = splitResponse[0];
                            arrReturn[0] = "INFO";
                        } else {
                            arrReturn[4] = "RobotSysLog";
                            arrReturn[0] = splitResponse[0];
                        }

                    } else {
                        arrReturn[2] = response;
                        arrReturn[0] = "INFO";
                        arrReturn[4] = "RobotSysLog";
                    }
                }
            } else {
                arrReturn[2] = response;
                arrReturn[0] = "INFO";
                arrReturn[4] = "RobotSysLog";
            }
        }
        return arrReturn;
    }
}

class LogViewConsumer implements Runnable {
    public static ServerSocket serverSocketLog = null;
    public static Socket       socketLog       = null;
    private InputStream        stream          = null;
    private BufferedReader     br              = null;
    private String             response        = null;
    int                        cnt             = 1;
    int                        cntNull         = 1;
    public static String       END_FLAG        = "#END_LOG#";
    private static LogView     mLogView;
    LogAppender                logAppender;

    public LogViewConsumer(int PORT, LogView logView) {
        try {
            serverSocketLog = new ServerSocket(PORT);
            mLogView = logView;

            LogHandler.startLogManager();
            System.out
                    .println("UILogViewConsumer wating LogManagerProducer...");

            logAppender = LogAppender.getInstance();
            if (logAppender.mInit == false) {
                logAppender.init(mLogView);
            }
            if (!logAppender.isAlive()) {
                logAppender.start();
            }

            socketLog = serverSocketLog.accept();
            stream = socketLog.getInputStream();
            br = new BufferedReader(new InputStreamReader(stream));
            System.out.println("UILogViewConsumer ready");
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("UILogViewConsumer init error :" + e.toString());
        }
    }

    @Override
    public void run() {
        while (true) {
            try {
                response = br.readLine();
                if (response != null) {
                    if (response.contains(END_FLAG)) {
                        System.out
                                .println("find end Flag, UILogViewConsumer end");
                        LogHandler.stopLogViewer();
                        break;
                    }
                    String log[] = LogHandler.convertLogMessage(response);
                    logAppender.addLog(new LogMessage(LogHandler
                            .convertLogLevel(log[0]), log[1], log[2], log[3],
                            log[4]));

                    System.out.println(cnt + " - " + response);
                    cnt++;
                } else {
                    cntNull++;
                    Thread.sleep(500);
                    if (cntNull == 10) {
                        System.out.println("UILogViewConsumer null error");
                        break;
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
                System.out.println("UILogViewConsumer error : " + e.toString());
                System.out.println("check log message : " + response);
                break;
            }
        }
        try {
            if (serverSocketLog != null) {
                serverSocketLog.close();
            }
            if (socketLog != null) {
                socketLog.close();
            }
            System.out.println("UILogViewConsumer socket close");
        } catch (Exception e) {
            e.printStackTrace();
            System.out
                    .println("UILogViewConsumer serverSocketLog close error : "
                            + e.toString());
        }
    }
}

class RealTimeLogViewConsumer implements Runnable {
    public static ServerSocket[] serverSocketLog = new ServerSocket[2];
    public static Socket         socketLog       = null;
    public InputStream           stream          = null;
    public BufferedReader        br              = null;
    private String               response        = null;
    static int                   cnt             = 1;
    public static int            cntSocket       = 0;
    int                          cntNull         = 1;
    public static final String   SPLIT           = "#s#";
    public static String         END_FLAG        = "#END_LOG#";
    private LogView              mView;
    private int                  mPort;
    public static Boolean        flag            = false;
    LogAppender                  logAppender;

    @Override
    public void run() {
        while (!LogHandler.stopRealtimeLogging) {
            try {
                response = br.readLine();
                if (response != null) {
                    System.out.println(cnt + " - live : " + response);
                    String log[] = LogHandler.convertLogMessage(response);
                    logAppender.addLog(new LogMessage(LogHandler
                            .convertLogLevel(log[0]), log[1], log[2], log[3],
                            log[4]));
                    cnt++;
                } else {
                    cntNull++;
                    Thread.sleep(500);
                    if (cntNull == 10) {
                        System.out
                                .println("UIRealTimeLogViewConsumer null error");
                        break;
                    }
                }
            } catch (Exception e) {
                // e.printStackTrace();
                System.out.println("UIRealTimeLogViewConsumer error : "
                        + e.toString());
                System.out.println("check log message : " + response);
                break;
            }
        }
    }
}

class LogAppender extends Thread {
    private List<LogMessage> mLogBuffer = new ArrayList<LogMessage>(
                                                LogMessageList.MAX_MESSAGES_DEFAULT);
    private List<LogMessage> logTemp    = new ArrayList<LogMessage>(
                                                LogMessageList.MAX_MESSAGES_DEFAULT);
    private LogPanel         mLogPanel;
    private int              time       = 200;                                       // ms
    public boolean           mInit;

    private LogAppender() {
        this.setDaemon(true);
        mInit = false;
    }

    private static class SingletonHolder {
        static final LogAppender logAppender = new LogAppender();
    }

    public static LogAppender getInstance() {
        return SingletonHolder.logAppender;
    }

    public void init(LogView logView) {
        this.mLogPanel = logView.mLogPanel;
        this.mInit = true;
    }

    public synchronized void addLog(LogMessage mLog) {
        this.mLogBuffer.add(mLog);
    }

    @Override
    public void run() {
        while (true) {
            if (!mLogBuffer.isEmpty()) {
                synchronized (mLogBuffer) {
                    logTemp.addAll(mLogBuffer);
                    mLogBuffer.clear();
                }
                mLogPanel.addLogMessage(logTemp);
                logTemp.clear();
            }
            try {
                Thread.sleep(time);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
