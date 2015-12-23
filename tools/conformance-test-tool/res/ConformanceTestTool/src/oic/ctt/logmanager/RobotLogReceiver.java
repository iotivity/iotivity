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

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.BlockingQueue;

import oic.ctt.ui.util.CTLogger;

import org.slf4j.Logger;

class RobotLogReceiver implements Runnable {
    private Logger                logger          = CTLogger.getInstance();
    private BlockingQueue<String> queue;
    public static ServerSocket    serverSocketLog = null;
    public static Socket          socketLog       = null;
    private InputStream           stream          = null;
    private BufferedReader        br              = null;
    private String                response        = null;
    int                           cnt             = 1;
    int                           cntNull         = 1;
    public static String          END_FLAG        = "#END_LOG#";

    public RobotLogReceiver(BlockingQueue<String> queue, int robotPort) {
        this.queue = queue;
        try {
            serverSocketLog = new ServerSocket(robotPort);
            logger.info("LogCollector RobotLogReceiver wating RobotLogProducer...");
            socketLog = serverSocketLog.accept();
            stream = socketLog.getInputStream();
            br = new BufferedReader(new InputStreamReader(stream));
            logger.info("LogCollector RobotLogReceiver ready");
        } catch (Exception e) {
            logger.info("LogCollector RobotLogReceiver init error :"
                    + e.toString());
        }
    }

    @Override
    public void run() {
        while (!LogCollector.stopCollector) {
            try {
                response = br.readLine();
                if (response != null) {
                    if (response.contains(END_FLAG)) {
                        queue.put(response);
                        logger.info("end LogCollector RobotLogReceiver");
                        break;
                    }
                    queue.put(response);

                } else {
                    cntNull++;
                    Thread.sleep(1000);
                    if (cntNull == 10) {
                        logger.info("LogCollector RobotLogReceiver null error");
                        break;
                    }
                }
                /*
                 * System.out.println(cnt++ + "[RobotLogReceiver][putTest] : " +
                 * response + "[" + queue.size() + "]");
                 */
            } catch (Exception e) {
                logger.info("LogCollector RobotLogReceiver error : "
                        + e.toString());
            }
        }
        try {
            if (serverSocketLog != null) {
                serverSocketLog.close();
            }
            if (socketLog != null) {
                socketLog.close();
            }
            logger.info("LogCollector RobotLogReceiver socket close");
        } catch (Exception e) {
            logger.info("LogCollector RobotLogReceiver serverSocketLog close error : "
                    + e.toString());
        }
    }
}