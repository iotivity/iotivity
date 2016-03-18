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

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;

import static oic.ctt.ui.types.ImageFilePathType.IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_FILTERS_TOOLBAR;
import static oic.ctt.ui.types.ToolTipTextType.TOOLTIP_TEXT_CREATE_RESOURCE_ACTION;
import oic.ctt.ui.Activator;
import oic.ctt.ui.multipages.ResourceCreator;
import oic.ctt.ui.multipages.ResourceEditorInput;

public class CreateResourceAction extends Action implements ISelectionListener,
		IWorkbenchAction {
	private IWorkbenchPage page;

	public CreateResourceAction(IWorkbenchPage workbenchPage) {
		super(TOOLTIP_TEXT_CREATE_RESOURCE_ACTION.toString());
		this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
				Activator.PLUGIN_ID,
				IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_FILTERS_TOOLBAR
						.toString()));
		this.setToolTipText(TOOLTIP_TEXT_CREATE_RESOURCE_ACTION.toString());
		page = workbenchPage;
	}

	@Override
	public void run() {
		try {
			IEditorInput idt = new ResourceEditorInput();
			IEditorReference[] editor = page.getEditorReferences();
			page.closeEditors(editor, false);
			page.openEditor(idt, ResourceCreator.ID, false);
		} catch (PartInitException e) {
			throw new RuntimeException(e);
		}
		return;
	}

	@Override
	public void dispose() {

	}

	@Override
	public void selectionChanged(IWorkbenchPart part, ISelection selection) {

	}

}
