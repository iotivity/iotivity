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

package oic.ctt.ui.views;


import java.io.File;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.part.ViewPart;
import org.slf4j.Logger;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.actions.DeleteAction;
import oic.ctt.ui.actions.ImportResourceFileAction;
import oic.ctt.ui.actions.MakeDescriptorAction;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.types.IDType.RAML_VIEW_ID;
import static oic.ctt.ui.types.ImageFilePathType.IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON;
import static oic.ctt.ui.types.ToolTipTextType.TOOLTIP_TEXT_REFRESH_VIEW;

public class RAMLView extends ViewPart {

	private Logger logger = CTLogger.getInstance();
	public CheckboxTreeViewer treeViewer;
	public static final String ID = RAML_VIEW_ID.toString();
	private static final String POPUP_MENU_TEXT = "#PopupMenu";

	@Override
	public void createPartControl(Composite parent) {
		final Action updateRAMLView = new Action(UIConst.REFRESH_ACTION_TEXT) {
			@Override
			public void run() {
				super.run();
				treeViewer.refresh();
			}
		};
		updateRAMLView.setToolTipText(TOOLTIP_TEXT_REFRESH_VIEW
				.toString());
		updateRAMLView
				.setImageDescriptor(Activator
						.getImageDescriptor(IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON
								.toString()));
		final MakeDescriptorAction makeDescriptorAction = new MakeDescriptorAction();
		final ImportResourceFileAction importResourceFileAction = new ImportResourceFileAction();
		final DeleteAction deleteAction = new DeleteAction(
				UIConst.RAML_PATH);
		getViewSite().getActionBars().getToolBarManager()
				.add(makeDescriptorAction);
		getViewSite().getActionBars().getToolBarManager()
				.add(importResourceFileAction);
		getViewSite().getActionBars().getToolBarManager().add(deleteAction);
		getViewSite().getActionBars().getToolBarManager()
		.add(updateRAMLView);
		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		IWorkspaceRoot root = workspace.getRoot();
		IProject newProjectHandle = root.getProject(UIConst.RAML_PATH);
		if (newProjectHandle != null) {
			logger.info("newProjectHandle.getLocation()="
					+ newProjectHandle.getLocation());

			treeViewer = new CheckboxTreeViewer(parent, SWT.BORDER | SWT.CHECK);

			if (treeViewer != null) {
				treeViewer
						.setContentProvider(new FileTreeContentProvider(this));
				treeViewer.setLabelProvider(new FileTreeLabelProvider(this));
				treeViewer.setSorter(new ViewerSorter());
				IPath newProjectHandleLocation = newProjectHandle.getLocation();
				if (newProjectHandleLocation != null) {
					if (treeViewer != null) {
						treeViewer.setInput(newProjectHandleLocation.toFile());
					}
				}
			}
		}
		MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT);
		Menu menu = menuMgr.createContextMenu(treeViewer.getTree());
		treeViewer.getTree().setMenu(menu);
		getSite().registerContextMenu(menuMgr, treeViewer);
	}

	@Override
	public void setFocus() {

	}

	public ISelection getSelectionTree() {
		return this.treeViewer.getSelection();
	}

	public void refreshTree() {
		this.treeViewer.refresh();
	}

	public void setFocusCurrentSuite(File suite) {
		treeViewer.setSelection(new StructuredSelection(suite));
	}

	public Object[] getSelectedItems() {
		CheckboxTreeViewer checkTreeViewer = (CheckboxTreeViewer) treeViewer;
		return checkTreeViewer.getCheckedElements();
	}

}
