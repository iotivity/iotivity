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
package oic.ctt.ui.actions;

import oic.ctt.ui.multipages.TestPlanSpecPage;
import static oic.ctt.ui.types.IDType.*;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;

public class UpdateTestPlanInfoAction extends Action implements
        IWorkbenchAction {

    public UpdateTestPlanInfoAction() {
        setId(UPDATE_TEST_PLAN_INFO_ID.toString());
    }

    public void run() {

        IWorkbenchPage page = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow().getActivePage();
        TestPlanSpecPage view = (TestPlanSpecPage) page
                .findView(TEST_PLAN_SPEC_PAGE_ID.toString());
        if (view != null) {
            view.updateCheckBox();
        }
    }

    public void dispose() {
    }

}
