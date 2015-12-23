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
package oic.ctt.ui.types;

public enum IDType {
    PLUGIN_ID("oic.ctt.ui"),
    START_TEST_ACTION_ID("oic.ctt.ui.actions.StartTestAction"),
    STOP_TEST_ACTION_ID("oic.ctt.ui.actions.StopTestAction"),
    CREATE_TEST_PLAN_ACTION_ID("oic.ctt.ui.actions.CreateTestPlan"),
    TEST_PLAN_SPEC_PAGE_ID("oic.ctt.ui.editors.TestPlanSpecPage"),
    REPORT_PAGE_EDITOR_ID("oic.ctt.ui.multipages.ReportPageEditor"),
    TEST_PLAN_MULTI_PAGE_EDITOR_ID("oic.ctt.ui.multipages.TestPlanMultiPageEditor"),
    TEST_SUITE_MULTI_PAGE_EDITOR_ID("oic.ctt.ui.multipages.TestSuiteMultiPageEditor"),
    TEST_SUITE_SPEC_PAGE_ID("oic.ctt.ui.editors.TestSuiteSpecPage"),
    LOG_VIEW_ID("oic.ctt.ui.views.LogView"),
    TEST_PLAN_VIEW_ID("oic.ctt.ui.views.TestPlanView"),
    TEST_REPORT_VIEW_ID("oic.ctt.ui.views.TestReportView"),
    TEST_SUITE_VIEW_ID("oic.ctt.ui.views.TestSuiteView"),
    PERSPECTIVE_ID("oic.ctt.ui.perspective"),
    UPDATE_TEST_PLAN_INFO_ID("oic.ctt.ui.actions.UpdateTestPlanInfo");

    private final String text;

    private IDType(String text) {
        this.text = text;
    }

    public String toString() {
       return this.text;
    }
}
