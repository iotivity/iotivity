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

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.BlockingQueue;

import oic.ctt.ui.util.CTLogger;

import org.slf4j.Logger;

public class LogManagerProducer implements Runnable {
	private static Logger logger = CTLogger.getInstance();
	private static int mPORT = 0;
	private BlockingQueue queue;
	private int cnt = 1;

	private static Socket socket = null;
	private static PrintWriter sendLog = null;
	private static boolean readySocketFail = false;
	private static boolean stopLogManagerProducer = false;
	private String response = null;
	public static String END_FLAG = "#END_LOG#";

	public LogManagerProducer(BlockingQueue queue, int port) {
		this.queue = queue;
		mPORT = port;
		stopLogManagerProducer = false;
		try {
			socket = new Socket("localhost", mPORT);
			logger.info("LogManagerProducer conneting LogViewConsumer...");
			sendLog = new PrintWriter(socket.getOutputStream(), true);
			logger.info("LogManagerProducer ready");
		} catch (IOException e) {
			readySocketFail = true;
			logger.info("check LogView Consumer port : " + e.toString());
		}
	}

	public static void stopLogManagerProducer() {
		stopLogManagerProducer = true;
		logger.info("LogManagerProducer Close");
	}

	@Override
	public void run() {
		while (!stopLogManagerProducer) {
			try {
				response = (String) queue.take();
				if (response != null) {
					if (response.contains(END_FLAG)) {
						sendLog.println(response);
						logger.info("LogManagerProducer end");
						break;
					}
					sendLog.println(response);
				}

			} catch (Exception e) {
				logger.info("LogManagerProducer msg sending error : "
						+ e.toString());
				logger.info("LogManagerProducer port : " + mPORT);
				break;
			}
		}
		try {
			if (!readySocketFail) {
				socket.close();
			}
		} catch (Exception e) {
			logger.info("LogManagerProducer socket.close() sending error : "
					+ e.toString());
		}
	}
}
