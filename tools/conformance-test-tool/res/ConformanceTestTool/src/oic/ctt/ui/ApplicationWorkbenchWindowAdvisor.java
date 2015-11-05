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

import org.eclipse.e4.ui.model.application.ui.SideValue;
import org.eclipse.e4.ui.model.application.ui.basic.MTrimBar;
import org.eclipse.e4.ui.model.application.ui.basic.MTrimmedWindow;
import org.eclipse.e4.ui.model.application.ui.basic.MWindow;
import org.eclipse.e4.ui.model.application.ui.menu.MToolControl;
import org.eclipse.e4.ui.workbench.modeling.EModelService;
import org.eclipse.swt.graphics.Point;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.WorkbenchException;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;
import org.eclipse.ui.application.IWorkbenchWindowConfigurer;
import org.eclipse.ui.application.WorkbenchWindowAdvisor;
import org.eclipse.ui.internal.WorkbenchWindow;

public class ApplicationWorkbenchWindowAdvisor extends WorkbenchWindowAdvisor {
    private static LogView mLogView;

    IWorkbenchWindow       window = (IWorkbenchWindow) PlatformUI
                                          .getWorkbench()
                                          .getActiveWorkbenchWindow();

    public ApplicationWorkbenchWindowAdvisor(
            IWorkbenchWindowConfigurer configurer) {
        super(configurer);
    }

    public ActionBarAdvisor createActionBarAdvisor(
            IActionBarConfigurer configurer) {
        return new ApplicationActionBarAdvisor(configurer);
    }

    public void preWindowOpen() {

        IWorkbenchWindowConfigurer configurer = getWindowConfigurer();
        configurer.setInitialSize(new Point(1350, 920));
        configurer.setShowMenuBar(true);
        configurer.setShowCoolBar(true);
        configurer.setShowStatusLine(true);

        configurer.setTitle("OIC Conformance Test Tool"); //$NON-NLS-1$
        configurer.setShowProgressIndicator(true);
    }

    public void postWindowOpen() {
        super.postWindowOpen();
        if (window instanceof WorkbenchWindow) {
            MWindow model = ((WorkbenchWindow) window).getModel();
            EModelService modelService = model.getContext().get(
                    EModelService.class);
            MToolControl searchField = (MToolControl) modelService.find(
                    "SearchField", model);
            if (searchField != null) {
                searchField.setToBeRendered(false);
                MTrimBar trimBar = modelService.getTrim((MTrimmedWindow) model,
                        SideValue.TOP);
                trimBar.getChildren().remove(searchField);
            }
        }

        IWorkbenchPage page = window.getActivePage();
        IViewPart part = null;
        try {
            part = page.showView(LogView.ID);
            page.activate(part);
        } catch (PartInitException e2) {
            e2.printStackTrace();
        }
    }

    public void postWindowClose() {

    }

    public void dispose() {
    }

    @Override
    public void openIntro() {

        try {
            window.getWorkbench().showPerspective(Perspective.ID, window);
        } catch (WorkbenchException e) {
            e.printStackTrace();
        }
        super.openIntro();
    }
}
