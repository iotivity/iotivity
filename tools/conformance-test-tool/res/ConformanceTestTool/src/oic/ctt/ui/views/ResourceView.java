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
import java.util.ArrayList;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.GroupMarker;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ListViewer;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

import oic.ctt.ui.util.ResourceProvider;
import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.actions.CreateResourceAction;
import oic.ctt.ui.actions.DeleteAction;
import static oic.ctt.ui.types.IDType.RESOURCE_VIEW_ID;
import static oic.ctt.ui.types.ImageFilePathType.IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON;
import static oic.ctt.ui.types.ToolTipTextType.TOOLTIP_TEXT_REFRESH_VIEW;
import static oic.ctt.ui.util.ResourceUtil.DESCRIPTOR_PATH;

public class ResourceView extends ViewPart {

	public ListViewer listViewer;
	public static final String ID = RESOURCE_VIEW_ID.toString();
	private static final String POPUP_MENU_TEXT = "#PopupMenu";

	@Override
	public void createPartControl(Composite parent) {

		final Action updateRersourceView = new Action(
				UIConst.REFRESH_ACTION_TEXT) {
			@Override
			public void run() {
				super.run();
				refreshResourceView();
			}
		};

		final IWorkbenchPage page = UIConst.getActivePage();
		updateRersourceView
				.setToolTipText(TOOLTIP_TEXT_REFRESH_VIEW.toString());
		updateRersourceView
				.setImageDescriptor(Activator
						.getImageDescriptor(IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON
								.toString()));

		final DeleteAction deleteAction = new DeleteAction(UIConst.RAML_PATH);
		final CreateResourceAction createResourceAction = new CreateResourceAction(
				page);
		getViewSite().getActionBars().getToolBarManager()
				.add(createResourceAction);
		getViewSite().getActionBars().getToolBarManager()
				.add(updateRersourceView);
		listViewer = new ListViewer(parent);

		if (listViewer != null) {
			refreshResourceView();
		}

		IDoubleClickListener doubleClickListener = new DoubleClickListenerThread(
				page, this);
		listViewer.addDoubleClickListener(doubleClickListener);
		MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT);
		menuMgr.addMenuListener(new IMenuListener() {
			@Override
			public void menuAboutToShow(IMenuManager manager) {
				manager.add(updateRersourceView);

				if (listViewer.getSelection() instanceof IStructuredSelection) {
					IStructuredSelection selection = (IStructuredSelection) listViewer
							.getSelection();
					boolean check = false;

					for (Object obj : selection.toList()) {
						if ((obj instanceof File)
								&& (((File) obj).isDirectory())) {
							check = true;
						}
					}

					if (check) {
						manager.add(deleteAction);
					}
				}
			}
		});

		menuMgr.add(new GroupMarker(IWorkbenchActionConstants.MB_ADDITIONS));
		getSite().registerContextMenu(menuMgr, listViewer);
		Control control = listViewer.getControl();
		Menu menu = menuMgr.createContextMenu(control);
		control.setMenu(menu);
		getSite().registerContextMenu(menuMgr, listViewer);

	}

	@Override
	public void setFocus() {
	}

	public ISelection getSelectionTree() {
		return this.listViewer.getSelection();
	}

	public void refreshResourceView() {
		ResourceProvider resourceProvider = new ResourceProvider(
				DESCRIPTOR_PATH);
		ArrayList<String> m_ResourceList = new ArrayList<String>();
		m_ResourceList = resourceProvider.getResourceList();
		listViewer.setContentProvider(new ArrayContentProvider());
		listViewer.setInput(m_ResourceList);
	}

	public void setFocusCurrentSuite(File suite) {
		listViewer.setSelection(new StructuredSelection(suite));
	}

}
