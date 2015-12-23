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

import oic.ctt.ui.util.CTLogger;
import oic.ctt.ui.views.LogView;

import org.slf4j.Logger;

class LogViewConsumer implements Runnable {
    private static Logger      logger          = CTLogger.getInstance();
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
            logger.info("UILogViewConsumer wating LogManagerProducer...");

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
            logger.info("UILogViewConsumer ready");
        } catch (Exception e) {
            e.printStackTrace();
            logger.info("UILogViewConsumer init error :" + e.toString());
        }
    }

    @Override
    public void run() {
        while (true) {
            try {
                response = br.readLine();
                if (response != null) {
                    if (response.contains(END_FLAG)) {
                        logger.info("find end Flag, UILogViewConsumer end");
                        LogHandler.stopLogViewer();
                        break;
                    }
                    String log[] = LogHandler.convertLogMessage(response);
                    logAppender.addLog(new LogMessage(LogHandler
                            .convertLogLevel(log[0]), log[1], log[2], log[3],
                            log[4]));
                    logger.info(cnt + " - " + response);
                    cnt++;
                } else {
                    cntNull++;
                    Thread.sleep(500);
                    if (cntNull == 10) {
                        logger.info("UILogViewConsumer null error");
                        break;
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
                logger.info("UILogViewConsumer error : " + e.toString());
                logger.info("check log message : " + response);
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
            logger.info("UILogViewConsumer socket close");
        } catch (Exception e) {
            e.printStackTrace();
            logger.info("UILogViewConsumer serverSocketLog close error : "
                    + e.toString());
        }
    }
}