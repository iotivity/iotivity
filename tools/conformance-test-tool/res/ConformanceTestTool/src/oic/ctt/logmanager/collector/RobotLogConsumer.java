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
package oic.ctt.logmanager.collector;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Map;

import oic.ctt.ui.util.CTLogger;

import org.slf4j.Logger;

public class RobotLogConsumer {
    private static final String END_TIME                   = "endtime";
    private static final String START_TIME                 = "starttime";
    private Logger              logger                     = CTLogger
                                                                   .getInstance();
    private static final String MESSAGE                    = "message";
    private static final String LEVEL                      = "level";
    private static final String TIMESTAMP                  = "timestamp";
    public static final int     ROBOT_LISTENER_API_VERSION = 2;
    public static final String  DEFAULT_FILENAME           = "listen_java.txt";
    public static final String  SPLIT                      = "#s#";
    public static final String  TAG_LOG_MSG                = "RobotLog";
    public static final String  TAG_SYSLOG_MSG             = "RobotSysLog";
    public static final String  REAR_MSG                   = "\n";
    public static final String  END_FLAG                   = "#END_LOG#";

    private static final int    LOGPORT                    = 3000;
    private Socket              socket                     = null;
    private BufferedWriter      outfile                    = null;
    private static PrintWriter  sendQueueLog               = null;

    public RobotLogConsumer() throws IOException {
        this(DEFAULT_FILENAME);
    }

    public RobotLogConsumer(String filename) throws IOException {
        String tmpdir = System.getProperty("java.io.tmpdir");
        String sep = System.getProperty("file.separator");
        String outpath = tmpdir + sep + filename;
        logger.info("====================================================");
        logger.info("robotlistener filel location : " + outpath);
        outfile = new BufferedWriter(new FileWriter(outpath));

        try {
            socket = new Socket("localhost", LOGPORT);
            sendQueueLog = new PrintWriter(socket.getOutputStream(), true);
            logger.info("RobotLogConsumer ready");
        } catch (Exception e) {
            logger.info("RobotLogConsumer init error : " + e.toString());
        }
    }

    public void close() {
        try {
            outfile.write(END_FLAG + "\n");
            sendQueueLog.println(END_FLAG);
            sendQueueLog.println(END_FLAG);
            outfile.close();
            socket.close();
            logger.info("RobotLogConsumer close");
        } catch (Exception e) {
            logger.info("RobotLogConsumer close error : " + e.toString());
        }
    }

    public void logMessage(Map attrs) throws IOException {
        try {
            // write listener file
            String log = attrs.get(TIMESTAMP) + SPLIT + attrs.get(LEVEL)
                    + SPLIT + TAG_LOG_MSG + SPLIT + attrs.get(MESSAGE);
            writeAndPrintLog(log);

        } catch (Exception e) {
            outfile.write("error : " + e.toString() + "\n");
        }
    }

    private void writeAndPrintLog(String log) throws IOException {
        try {
            outfile.write(log + "\n");
            sendQueueLog.println(log);
        } catch (Exception e) {
            outfile.write("error : " + e.toString() + "\n");
        }
    }

    // robotframework system log
    public void message(Map attrs) throws IOException {
        try {
            // write listener file
            String log = attrs.get(TIMESTAMP) + SPLIT + attrs.get(LEVEL)
                    + SPLIT + TAG_SYSLOG_MSG + SPLIT + attrs.get(MESSAGE);
            writeAndPrintLog(log);

        } catch (Exception e) {
            outfile.write("error : " + e.toString() + "\n");
        }
    }

    public void startSuite(String name, Map attrs) throws IOException {
        try {
            String tagging = attrs.get(START_TIME) + SPLIT + TAG_LOG_MSG;

            String log = TAG_LOG_MSG
                    + SPLIT
                    + "=======================================================================================\n"
                    + tagging
                    + SPLIT
                    + "Start Suite : "
                    + name
                    + ", Total tests : "
                    + attrs.get("totaltests")
                    + "\n"
                    + TAG_LOG_MSG
                    + SPLIT
                    + "=======================================================================================";

            writeAndPrintLog(log);

        } catch (Exception e) {
            outfile.write("error : " + e.toString() + "\n");
        }
    }

    public void startTest(String name, Map attrs) throws IOException {
        try {
            String tagging = attrs.get(START_TIME) + SPLIT + TAG_LOG_MSG;

            String log = TAG_LOG_MSG
                    + SPLIT
                    + "---------------------------------------------------------------------------------------\n"
                    + tagging
                    + SPLIT
                    + "Start Test : "
                    + name
                    + "\n"
                    + TAG_LOG_MSG
                    + SPLIT
                    + "---------------------------------------------------------------------------------------";

            writeAndPrintLog(log);

        } catch (Exception e) {
            outfile.write("error : " + e.toString() + "\n");
        }
    }

    public void endTest(String name, Map attrs) throws IOException {
        /*
         * String status = attrs.get("status").toString(); if
         * (status.equals("PASS")) { outfile.write("PASS\n"); } else {
         * outfile.write("FAIL: " + attrs.get("message") + "\n"); }
         */
        String tagging = attrs.get("tags").toString();
        // String tcname = attrs.get("longname").toString();
        outfile.write("###tag : " + tagging + "\n");
    }

    public void endSuite(String name, Map attrs) throws IOException {
        try {
            String tagging = attrs.get(END_TIME) + SPLIT + TAG_LOG_MSG;

            String log = TAG_LOG_MSG
                    + SPLIT
                    + "=======================================================================================\n"
                    + tagging
                    + SPLIT
                    + "End Suite : "
                    + name
                    + ", Elapsed time : "
                    + attrs.get("elapsedtime")
                    + "ms\n"
                    + TAG_LOG_MSG
                    + SPLIT
                    + "Tests execution ended. Statistics : "
                    + attrs.get("statistics")
                    + "\n"
                    + TAG_LOG_MSG
                    + SPLIT
                    + "=======================================================================================";

            writeAndPrintLog(log);

        } catch (Exception e) {
            outfile.write("error : " + e.toString() + "\n");
        }
    }
}