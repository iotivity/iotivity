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
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 0
package oic.ctt.ui.log;

import java.util.ArrayList;
import java.util.List;

import oic.ctt.ui.views.LogView;

class LogAppender extends Thread {
    private List<LogMessage> mLogBuffer = new ArrayList<LogMessage>(
                                                LogMessageList.MAX_MESSAGES_DEFAULT);
    private List<LogMessage> logTemp    = new ArrayList<LogMessage>(
                                                LogMessageList.MAX_MESSAGES_DEFAULT);
    private LogPanel         mLogPanel;
    private int              time       = 200;                                       // ms
    public boolean           mInit;

    private LogAppender() {
        this.setDaemon(true);
        mInit = false;
    }

    private static class SingletonHolder {
        static final LogAppender logAppender = new LogAppender();
    }

    public static LogAppender getInstance() {
        return SingletonHolder.logAppender;
    }

    public void init(LogView logView) {
        this.mLogPanel = logView.mLogPanel;
        this.mInit = true;
    }

    public synchronized void addLog(LogMessage mLog) {
        this.mLogBuffer.add(mLog);
    }

    @Override
    public void run() {
        while (true) {
            if (!mLogBuffer.isEmpty()) {
                synchronized (mLogBuffer) {
                    logTemp.addAll(mLogBuffer);
                    mLogBuffer.clear();
                }
                mLogPanel.addLogMessage(logTemp);
                logTemp.clear();
            }
            try {
                Thread.sleep(time);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}