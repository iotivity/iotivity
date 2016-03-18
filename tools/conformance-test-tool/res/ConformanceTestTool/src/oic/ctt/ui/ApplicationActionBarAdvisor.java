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

import oic.ctt.ui.actions.PauseTestAction;
import oic.ctt.ui.actions.StartTestAction;
import oic.ctt.ui.actions.StopTestAction;
import oic.ctt.ui.multipages.TestPlanMultiPageEditor;
import oic.ctt.ui.multipages.TestPlanSpecPage;

import org.eclipse.core.runtime.IExtension;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.ICoolBarManager;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.ToolBarContributionItem;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IPartListener2;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPartReference;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;
import org.eclipse.ui.internal.WorkbenchPlugin;
import org.eclipse.ui.internal.registry.ActionSetRegistry;
import org.eclipse.ui.internal.registry.IActionSetDescriptor;

public class ApplicationActionBarAdvisor extends ActionBarAdvisor {
    private IWorkbenchWindow window;
    private IWorkbenchPage   page;
    private IWorkbenchAction exitAction;
    private IWorkbenchAction aboutAction;
    private IWorkbenchAction restoreAction;
    private IWorkbenchAction preferencesAction;
    private Action           startTestAction;
    private Action           stopTestAction;
    private Action           pauseTestAction;

    public ApplicationActionBarAdvisor(IActionBarConfigurer configurer) {
        super(configurer);
        removeUnWantedActions();
        window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        page = window.getActivePage();
        page.addPartListener(new IPartListener2() {

            @Override
            public void partActivated(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
                String currentPerspective = page.getPerspective().getId();
                if (currentPerspective.equals(Perspective.ID)) {
                    if (existRuningJob() == false) {
                        if (paramIWorkbenchPartReference != null) {
                            if (paramIWorkbenchPartReference.getPage()
                                    .getActiveEditor() instanceof TestPlanMultiPageEditor) {
                                IWorkbenchPage workbenchPage = paramIWorkbenchPartReference
                                        .getPage();
                                if (workbenchPage != null) {
                                    TestPlanMultiPageEditor multipage = (TestPlanMultiPageEditor) workbenchPage
                                            .getActiveEditor();
                                    if (multipage != null) {
                                        TestPlanSpecPage specpage = (TestPlanSpecPage) multipage
                                                .getActivePageInstance();
                                        startTestAction.setEnabled(true);
                                    }
                                }
                            } else {
                                startTestAction.setEnabled(false);
                            }
                        }
                    }
                }

            }

            @Override
            public void partBroughtToTop(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
            }

            @Override
            public void partClosed(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
                if (page.getPerspective() != null) {
                    String currentPerspective = page.getPerspective().getId();
                    if (currentPerspective.equals(Perspective.ID)) {
                        if (paramIWorkbenchPartReference.getPage()
                                .getEditorReferences().length == 0) {
                            startTestAction.setEnabled(false);
                        }
                    }
                }

            }

            @Override
            public void partDeactivated(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
            }

            @Override
            public void partHidden(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
            }

            @Override
            public void partInputChanged(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
            }

            @Override
            public void partOpened(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
            }

            @Override
            public void partVisible(
                    IWorkbenchPartReference paramIWorkbenchPartReference) {
            }
        });
    }

    private boolean existRuningJob() {
        boolean existJob = false;
        Job[] jobs = Job.getJobManager().find(null);
        for (Job job : jobs) {
            if (job.getClass().toString()
                    .startsWith("class oic.ctt.ui.actions.StartTestAction")) {
                existJob = true;
                break;
            }
        }
        return existJob;
    }

    protected void makeActions(IWorkbenchWindow window) {
        this.window = window;

        exitAction = ActionFactory.QUIT.create(window);
        register(exitAction);

        aboutAction = ActionFactory.ABOUT.create(window);
        register(aboutAction);

        startTestAction = new StartTestAction(window);
        startTestAction.setEnabled(false);
        stopTestAction = new StopTestAction(window);
        pauseTestAction = new PauseTestAction(window);
        restoreAction = ActionFactory.RESET_PERSPECTIVE.create(window);
        register(restoreAction);
        preferencesAction = ActionFactory.PREFERENCES.create(window);
        register(preferencesAction);

    }

    protected void fillMenuBar(IMenuManager menuBar) {
        MenuManager fileMenu = new MenuManager("&File", Activator.PLUGIN_ID);
        fileMenu.add(exitAction);

        menuBar.add(fileMenu);

        MenuManager runMenu = new MenuManager("&Run", Activator.PLUGIN_ID);
        menuBar.add(runMenu);
        // progress
        runMenu.add(startTestAction);
        runMenu.add(stopTestAction);
        runMenu.add(pauseTestAction);

        MenuManager windowMenu = new MenuManager("&Window",
                IWorkbenchActionConstants.M_WINDOW);
        menuBar.add(windowMenu);
        windowMenu.add(restoreAction);
        MenuManager showViewMenu = new MenuManager("Show View");
        IContributionItem showViewList = ContributionItemFactory.VIEWS_SHORTLIST
                .create(window);
        showViewMenu.add(showViewList);
        windowMenu.add(showViewMenu);
        windowMenu.add(new Separator());
        windowMenu.add(preferencesAction);

        MenuManager helpMenu = new MenuManager("&Help", Activator.PLUGIN_ID);
        menuBar.add(helpMenu);
        helpMenu.add(aboutAction);
    }

    protected void fillCoolBar(ICoolBarManager coolBar) {

        IToolBarManager toolbar = new ToolBarManager(SWT.FLAT | SWT.RIGHT);
        coolBar.add(new ToolBarContributionItem(toolbar, "main"));

        toolbar.add(new Separator());

        ActionContributionItem startTest = new ActionContributionItem(
                startTestAction);
        startTest.setMode(ActionContributionItem.MODE_FORCE_TEXT);
        toolbar.add(startTest);

        ActionContributionItem stopTest = new ActionContributionItem(
                stopTestAction);
        stopTest.setMode(ActionContributionItem.MODE_FORCE_TEXT);
        toolbar.add(stopTest);

        ActionContributionItem pauseTest = new ActionContributionItem(
                pauseTestAction);
        pauseTest.setMode(ActionContributionItem.MODE_FORCE_TEXT);
        toolbar.add(pauseTest);

        toolbar.add(new Separator());
    }

    @SuppressWarnings({ "restriction", "unused" })
    private void removeUnWantedActions() {
        final String[] actionSetId = new String[] {
                "org.eclipse.search.searchActionSet",
                "org.eclipse.ui.edit.text.actionSet.annotationNavigation",
                "org.eclipse.ui.edit.text.actionSet.navigation",
                "org.eclipse.jdt.ui.SearchActionSet", };
        ActionSetRegistry asr = WorkbenchPlugin.getDefault()
                .getActionSetRegistry();
        IActionSetDescriptor[] actionSets = asr.getActionSets();
        String id = IWorkbenchActionConstants.PRINT_EXT;
        IWorkbenchWindow window = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow();

        IExtension ext = null;
        for (IActionSetDescriptor actionSet : actionSets) {
            for (String element : actionSetId) {
                if (element.equals(actionSet.getId())) {
                    ext = actionSet.getConfigurationElement()
                            .getDeclaringExtension();
                    asr.removeExtension(ext, new Object[] { actionSet });
                }
            }
        }
    }

}
