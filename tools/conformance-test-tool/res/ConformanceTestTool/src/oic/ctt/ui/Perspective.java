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
package oic.ctt.ui;

import oic.ctt.ui.views.LogView;
import oic.ctt.ui.views.TestPlanView;
import oic.ctt.ui.views.TestReportView;
import oic.ctt.ui.views.TestSuiteView;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;
import org.eclipse.ui.IViewLayout;

public class Perspective implements IPerspectiveFactory {

    public static final String ID = "oic.ctt.ui.perspective";

    public void createInitialLayout(IPageLayout layout) {
        layout.setEditorAreaVisible(true);
        layout.setFixed(true);

        String editorArea = layout.getEditorArea();

        IFolderLayout bottomMiddle = layout.createFolder("bottomMiddle",
                IPageLayout.BOTTOM, 0.65f, editorArea);

        bottomMiddle.addView(LogView.ID);

        IFolderLayout left = layout.createFolder("left", IPageLayout.LEFT,
                0.25f, editorArea);
        left.addView(TestSuiteView.ID);
        left.addView(TestPlanView.ID);
        left.addView(TestReportView.ID);

        IViewLayout testSuiteViewLayout = layout
                .getViewLayout(TestSuiteView.ID);
        if (testSuiteViewLayout != null) {
            testSuiteViewLayout.setCloseable(false);
        }
        IViewLayout logViewLayout = layout.getViewLayout(LogView.ID);
        if (logViewLayout != null) {
            logViewLayout.setCloseable(false);
        }

        layout.addPerspectiveShortcut(Activator.PLUGIN_ID);

        layout.addShowViewShortcut(TestSuiteView.ID);
        layout.addShowViewShortcut(TestPlanView.ID);
        layout.addShowViewShortcut(TestReportView.ID);
        layout.addShowViewShortcut(LogView.ID);

        UIConst.createProject(UIConst.TESTSUITE_PATH);
        UIConst.createProject(UIConst.TESTPLAN_PATH);
        UIConst.createProject(UIConst.TESTREPORT_PATH);
    }
}
