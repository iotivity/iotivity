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
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class LogCollector {
    private final static int    PORT          = 3000;
    public static BlockingQueue mergeQueue;
    public static Thread        robotThread;
    public static final String  SPLIT         = "#s#";
    public static Boolean       stopCollector = false;

    public static void startLogCollector() {
        mergeQueue = new ArrayBlockingQueue(100000);

        robotThread = new Thread(new RobotLogReceiver(mergeQueue, PORT));
        stopCollector = false;
        try {
            Thread.sleep(500);
            robotThread.start();
            System.out.println("startLogCollector");
        } catch (Exception e) {
            System.out.println("MergeQueue startLogCollector :" + e.toString());
        }
    }

    public static void stopLogCollector() {
        stopCollector = true;
        System.out.println("stopLogCollector");
    }

    public static String setNomalizeTag(String tag) {
        String temp = null;

        if (tag == null) {
            temp = "NoTag";
        } else if (tag.contains("tc")) {
            temp = "TestScript";
        } else if (tag.contains("ZWave")) {
            temp = "ZWave";
        } else if (tag.contains("ZigBee")) {
            temp = "ZigBee";
        } else {
            temp = tag;
        }
        return temp + SPLIT;
    }

    public static String[] setNomalizeThingsMsg(String response) {
        // 0 20150918 14:54:06.491#s#
        // 1 INFO#s#
        // 2 RobotLog#s#

        // 1 20150918 14:53:56.548#s#
        // 2 #s#INFO
        // 3 #s#ZigBee
        // 4 #s#[Thread-9]:
        String[] splitResponse = null;
        // level, tag, msg
        String[] arrReturn = { "", "", "" };

        try {
            if (response != null) {
                if (response.contains(SPLIT)) {
                    splitResponse = response.split(SPLIT);
                    arrReturn[0] = splitResponse[3];
                    arrReturn[1] = splitResponse[1];
                    arrReturn[2] = splitResponse[2] + SPLIT + splitResponse[4];
                } else {
                    arrReturn[0] = "INFO";
                    arrReturn[1] = " ";
                    arrReturn[2] = response;
                }
            }
        } catch (Exception e) {
            System.out.println("setNomalizeThingsMsg error : " + e.toString());
            e.printStackTrace();
        }
        return arrReturn;
    }
}

class RobotLogReceiver implements Runnable {
    private BlockingQueue      queue;
    public static ServerSocket serverSocketLog = null;
    public static Socket       socketLog       = null;
    private InputStream        stream          = null;
    private BufferedReader     br              = null;
    private String             response        = null;
    int                        cnt             = 1;
    int                        cntNull         = 1;
    public static String       END_FLAG        = "#END_LOG#";

    public RobotLogReceiver(BlockingQueue queue, int robotPort) {
        this.queue = queue;
        try {
            serverSocketLog = new ServerSocket(robotPort);
            System.out
                    .println("LogCollector RobotLogReceiver wating RobotLogProducer...");
            socketLog = serverSocketLog.accept();

            stream = socketLog.getInputStream();
            br = new BufferedReader(new InputStreamReader(stream));
            System.out.println("LogCollector RobotLogReceiver ready");
        } catch (Exception e) {
            System.out.println("LogCollector RobotLogReceiver init error :"
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
                        System.out.println("end LogCollector RobotLogReceiver");
                        break;
                    }
                    queue.put(response);

                } else {
                    cntNull++;
                    Thread.sleep(1000);
                    if (cntNull == 10) {
                        System.out
                                .println("LogCollector RobotLogReceiver null error");
                        break;
                    }
                }
                /*
                 * System.out.println(cnt++ + "[RobotLogReceiver][putTest] : " +
                 * response + "[" + queue.size() + "]");
                 */
            } catch (Exception e) {
                System.out.println("LogCollector RobotLogReceiver error : "
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

            System.out.println("LogCollector RobotLogReceiver socket close");
        } catch (Exception e) {
            System.out
                    .println("LogCollector RobotLogReceiver serverSocketLog close error : "
                            + e.toString());
        }
    }
}

// another thread for external log
class LogReceiver implements Runnable {
    private BlockingQueue  queue;
    private ServerSocket   serverSocketLog = null;
    private static Socket  socketLog       = null;
    private InputStream    stream          = null;
    private BufferedReader br              = null;
    private String         response        = null;
    int                    cnt             = 1;

    public LogReceiver(BlockingQueue queue, int port,
            ServerSocket serverSocketLog) {
        this.queue = queue;
        this.serverSocketLog = serverSocketLog;

        try {
            serverSocketLog = new ServerSocket(port);
            System.out.println("LogReceiver wating LogProducer...");
            socketLog = serverSocketLog.accept();
            stream = socketLog.getInputStream();
            br = new BufferedReader(new InputStreamReader(stream));
            System.out.println("LogReceiver ready");
        } catch (Exception e) {
            System.out.println("LogReceiver init error : " + e.toString());
        }

    }

    @Override
    public void run() {
        while (true) {
            try {
                response = br.readLine();
                if (response == null) {
                    System.out.println("end LogReceiver");
                    break;
                } else {
                    queue.put(response);
                }
                System.out.println(cnt++ + ": [" + queue.size() + "] : ");
            } catch (Exception e) {
                System.out.println("LogReceiver error : " + e.toString());
            }
        }

        try {
            serverSocketLog.close();
            socketLog.close();
        } catch (Exception e) {
            System.out.println("LogReceiver serverSocketLog close error : "
                    + e.toString());
        }
    }
}
