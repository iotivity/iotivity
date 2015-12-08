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
package oic.ctt.testhelper;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import org.junit.internal.AssumptionViolatedException;
import org.junit.rules.TestWatcher;
import org.junit.runner.Description;

public class ReportGenerator extends TestWatcher {

    private RMReportManager               rptMgr;
    private String                        testsuiteName;
    private String                        testcaseName;
    private String                        failureMessage;
    private String                        packageName;
    private String                        testDuration;
    private String                        status;
    private Calendar                      now;
    private Calendar                      then;
    private HashMap<String, List<String>> testList;
    private List<String>                  testValue;
    private String                        testLog;
    private String                        testSpec;

    public ReportGenerator(String packageName) {
        this.packageName = packageName;
        this.rptMgr = new RMReportManager();
        this.testList = new HashMap<String, List<String>>();
        this.testValue = new ArrayList<String>();
    }

    public ReportGenerator() {
        this(null);
    }

    @Override
    protected void starting(Description desc) {
        if (desc.getTestClass() != null) {
            testsuiteName = desc.getTestClass().getSimpleName();
            testcaseName = desc.getMethodName();
            testSpec = formatTestSpec(desc);
            this.now = Calendar.getInstance();
            this.now.setTime(new Date());
            testLog = testLog + testcaseName + "\n";
        }
    }

    @Override
    protected void succeeded(Description description) {
        status = "run";
    }

    @Override
    protected void failed(Throwable e, Description description) {
        status = "run";
        StackTraceElement[] elements = e.getStackTrace();
        failureMessage = e.getMessage() + "\n";
        for (int i = 0; i < 6; i++) {
            failureMessage = failureMessage.concat(elements[i].toString()
                    + "\n");
        }
    }

    @Override
    protected void skipped(AssumptionViolatedException e,
            Description description) {
    }

    @Override
    protected void finished(Description description) {
        this.then = Calendar.getInstance();
        this.then.setTime(new Date());
        testDuration = Long.toString(this.then.getTimeInMillis()
                - this.now.getTimeInMillis());
        testValue.add(testcaseName);
        testValue.add(status);
        testValue.add(testDuration);
        testValue.add(failureMessage);
        testValue.add(testSpec);
        testList.put(testcaseName, testValue);
        // rptMgr.writeLog(testLog, packageName);
        cleanUp();
        finishSuite();
    }

    private void cleanUp() {
        testcaseName = null;
        status = null;
        failureMessage = null;
        testDuration = null;
        this.now = null;
        this.then = null;
        testLog = null;
        testSpec = null;
    }

    private void finishSuite() {
        rptMgr.createTestReport(testList, testsuiteName, packageName);
        testsuiteName = "";
        testList.clear();
    }

    private String formatTestSpec(Description desc) {
        if (desc == null) {
            return "Description object getting null";
        }
        Method m;
        try {
            if (desc.getTestClass() != null) {
                m = desc.getTestClass().getMethod(testcaseName);
                if (m != null) {
                    testSpec = m.getAnnotations()[0].toString();
                }
            }
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (SecurityException e) {
            e.printStackTrace();
        }

        testSpec = testSpec.substring(testSpec.indexOf("(") + 1);
        testSpec = testSpec.substring(0, testSpec.length() - 1);
        StringBuffer tempTestSpec = new StringBuffer();
        String[] temp = testSpec.split(",");
        for (String value : temp) {
            tempTestSpec.append("\n" + value);
        }
        return tempTestSpec.toString();
    }

}
