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
import java.net.ServerSocket;
import java.net.Socket;

import oic.ctt.ui.util.CTLogger;
import org.slf4j.Logger;

class RealTimeLogViewConsumer implements Runnable {
    private static Logger        logger          = CTLogger.getInstance();
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
    public static Boolean        flag            = false;
    LogAppender                  logAppender;

    @Override
    public void run() {
        while (!LogHandler.stopRealtimeLogging) {
            try {
                response = br.readLine();
                if (response != null) {
                    logger.info(cnt + " - live : " + response);
                    String log[] = LogHandler.convertLogMessage(response);
                    logAppender.addLog(new LogMessage(LogHandler
                            .convertLogLevel(log[0]), log[1], log[2], log[3],
                            log[4]));
                    cnt++;
                } else {
                    cntNull++;
                    Thread.sleep(500);
                    if (cntNull == 10) {
                        logger.info("UIRealTimeLogViewConsumer null error");
                        break;
                    }
                }
            } catch (Exception e) {
                // e.printStackTrace();
                logger.info("UIRealTimeLogViewConsumer error : " + e.toString());
                logger.info("check log message : " + response);
                break;
            }
        }
    }
}