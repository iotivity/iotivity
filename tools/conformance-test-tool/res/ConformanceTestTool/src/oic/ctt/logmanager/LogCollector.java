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

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import oic.ctt.ui.util.CTLogger;

import org.slf4j.Logger;

public class LogCollector {
    private static final String         ZIG_BEE       = "ZigBee";
    private static final String         Z_WAVE        = "ZWave";
    private static final String         TEST_SCRIPT   = "TestScript";
    private static final String         TC            = "tc";
    private static final String         NO_TAG        = "NoTag";
    private static Logger               logger        = CTLogger.getInstance();
    private final static int            PORT          = 3000;
    public static BlockingQueue<String> mergeQueue;
    public static Thread                robotThread;
    public static final String          SPLIT         = "#s#";
    public static Boolean               stopCollector = false;

    public static void startLogCollector() {
        mergeQueue = new ArrayBlockingQueue<String>(100000);

        robotThread = new Thread(new RobotLogReceiver(mergeQueue, PORT));
        stopCollector = false;
        try {
            Thread.sleep(500);
            robotThread.start();
            logger.info("startLogCollector");
        } catch (Exception e) {
            logger.info("MergeQueue startLogCollector :" + e.toString());
        }
    }

    public static void stopLogCollector() {
        stopCollector = true;
        logger.info("stopLogCollector");
    }

    public static String setNomalizeTag(String tag) {
        String temp = null;

        if (tag == null) {
            temp = NO_TAG;
        } else if (tag.contains(TC)) {
            temp = TEST_SCRIPT;
        } else if (tag.contains(Z_WAVE)) {
            temp = Z_WAVE;
        } else if (tag.contains(ZIG_BEE)) {
            temp = ZIG_BEE;
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
            logger.info("setNomalizeThingsMsg error : " + e.toString());
            e.printStackTrace();
        }
        return arrReturn;
    }
}
