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
package oic.ctt.ui.multipages;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.part.MultiPageEditorActionBarContributor;
import org.eclipse.ui.texteditor.ITextEditor;

/**
 * Manages the installation/deinstallation of global actions for multi-page
 * editors. Responsible for the redirection of global actions to the active
 * editor. Multi-page contributor replaces the contributors for the individual
 * editors in the multi-page editor.
 */
public class MultiPageEditorContributor extends
        MultiPageEditorActionBarContributor {

    private IEditorPart      activeEditorPart;
    // private Action sampleAction;
    private IWorkbenchAction saveAction;
    private IWorkbenchAction saveAllAction;
    private IWorkbenchAction undoAction;
    private IWorkbenchAction redoAction;

    /**
     * Creates a multi-page contributor.
     */
    public MultiPageEditorContributor() {
        super();
        createActions();
    }

    /**
     * Returns the action registed with the given text editor.
     * 
     * @return IAction or null if editor is null.
     */
    protected IAction getAction(ITextEditor editor, String actionID) {
        return (editor == null ? null : editor.getAction(actionID));
    }

    /*
     * (non-JavaDoc) Method declared in
     * AbstractMultiPageEditorActionBarContributor.
     */
    public void setActivePage(IEditorPart part) {

        if (activeEditorPart == part)
            return;
        activeEditorPart = part;
    }

    private void createActions() {
        IWorkbenchWindow window = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow();
        saveAction = ActionFactory.SAVE.create(window);
        saveAllAction = ActionFactory.SAVE_ALL.create(window);
        undoAction = ActionFactory.UNDO.create(window);
        redoAction = ActionFactory.REDO.create(window);

        IActionBars actionBars = getActionBars();
        if (actionBars != null) {
            actionBars.setGlobalActionHandler(ActionFactory.SAVE.getId(),
                    saveAction);
            actionBars.setGlobalActionHandler(ActionFactory.SAVE_ALL.getId(),
                    saveAllAction);
            actionBars.setGlobalActionHandler(ActionFactory.UNDO.getId(),
                    undoAction);
            actionBars.setGlobalActionHandler(ActionFactory.REDO.getId(),
                    redoAction);
        }
    }

    public void contributeToMenu(IMenuManager manager) {
    }

    public void contributeToToolBar(IToolBarManager manager) {
        manager.add(new Separator());
        manager.add(saveAction);
        manager.add(saveAllAction);
        manager.add(undoAction);
        manager.add(redoAction);
    }

}
