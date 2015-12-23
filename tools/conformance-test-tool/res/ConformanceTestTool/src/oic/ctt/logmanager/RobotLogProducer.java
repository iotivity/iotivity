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

import oic.ctt.ui.util.CTLogger;

import org.slf4j.Logger;

public class RobotLogProducer {

	private static Logger logger = CTLogger.getInstance();
	private static final String NO_MESSAGE = "No message";
	private static String info = "*INFO* ";
	private static String debug = "*DEBUG* ";
	private static String warn = "*WARN* ";
	private static String error = "*ERROR* ";
	private static String trace = "*TRACE* ";
	private static String sendingLog = null;
	public static final String SPLIT = "#s#";

	public RobotLogProducer() {
	}

	public static void info_log(String tag, String msg) {

		if (msg == null) {
			msg = NO_MESSAGE;
		}

		sendLog(info, tag, msg);
	}

	public static void debug_log(String tag, String msg) {
		if (msg == null) {
			msg = NO_MESSAGE;
		}

		sendLog(debug, tag, msg);
	}

	public static void warn_log(String tag, String msg) {
		if (msg == null) {
			msg = NO_MESSAGE;
		}

		sendLog(warn, tag, msg);
	}

	public static void trace_log(String tag, String msg) {
		if (msg == null) {
			msg = NO_MESSAGE;
		}

		sendLog(trace, tag, msg);
	}

	public static void error_log(String tag, String msg) {
		if (msg == null) {
			msg = NO_MESSAGE;
		}

		sendLog(error, tag, msg);
	}

	private static void sendLog(String str, String tag, String msg) {
		sendingLog = str + LogCollector.setNomalizeTag(tag) + "" + msg;
		logger.info(sendingLog);
	}
}