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

class LogReceiver implements Runnable {
    private Logger                logger          = CTLogger.getInstance();
    private BlockingQueue<String> queue;
    private ServerSocket          serverSocketLog = null;
    private static Socket         socketLog       = null;
    private InputStream           stream          = null;
    private BufferedReader        br              = null;
    private String                response        = null;
    int                           cnt             = 1;

    public LogReceiver(BlockingQueue<String> queue, int port,
            ServerSocket serverSocketLog) {
        this.queue = queue;
        this.serverSocketLog = serverSocketLog;

        try {
            serverSocketLog = new ServerSocket(port);
            logger.info("LogReceiver wating LogProducer...");
            socketLog = serverSocketLog.accept();
            stream = socketLog.getInputStream();
            br = new BufferedReader(new InputStreamReader(stream));
            logger.info("LogReceiver ready");
        } catch (Exception e) {
            logger.info("LogReceiver init error : " + e.toString());
        }

    }

    @Override
    public void run() {
        while (true) {
            try {
                response = br.readLine();
                if (response == null) {
                    logger.info("end LogReceiver");
                    break;
                } else {
                    queue.put(response);
                }
                logger.info(cnt++ + ": [" + queue.size() + "] : ");
            } catch (Exception e) {
                logger.info("LogReceiver error : " + e.toString());
            }
        }

        try {
            serverSocketLog.close();
            socketLog.close();
        } catch (Exception e) {
            logger.info("LogReceiver serverSocketLog close error : "
                    + e.toString());
        }
    }
}