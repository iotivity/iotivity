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

import com.android.ddmlib.Log.LogLevel;

/**
 * Model a single log message output from {@code logcat -v long}. A logcat
 * message has a {@link LogLevel}, the pid (process id) of the process
 * generating the message, the time at which the message was generated, and the
 * tag and message itself.
 */
public final class LogMessage {
    private final LogLevel mLogLevel;
    private final String   mTime;
    private final String   mMessage;
    private final String   mTag;
    private final String   mRobotSysLog;

    /**
     * Construct an immutable log message object.
     */
    public LogMessage(LogLevel logLevel, String time, String msg, String tag,
            String robotSysLog) {
        mLogLevel = logLevel;
        mTime = time;
        mMessage = msg;
        mTag = tag;
        mRobotSysLog = robotSysLog;
    }

    public LogLevel getLogLevel() {
        return mLogLevel;
    }

    public String getTime() {
        return mTime;
    }

    public String getMessage() {
        return mMessage;
    }

    public String getTag() {
        return mTag;
    }

    public String getRobotSysLog() {
        return mRobotSysLog;
    }

    @Override
    public String toString() {
        return mTime + ": " + mLogLevel.getPriorityLetter() + "/" + mTag
                + mMessage + mRobotSysLog;
    }
}
