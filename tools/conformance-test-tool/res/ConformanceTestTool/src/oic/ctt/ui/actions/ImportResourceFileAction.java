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

import java.io.File;
import java.io.IOException;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.slf4j.Logger;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.CTLogger;
import oic.ctt.ui.views.RAMLView;
import static oic.ctt.ui.types.ImageFilePathType.IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_TEST_SUITE;
import static oic.ctt.ui.types.ToolTipTextType.TOOLTIP_TEXT_IMPORT_RESOURCE_FILE;
import static oic.ctt.ui.util.PopUpUtil.*;

public class ImportResourceFileAction extends Action implements
		ISelectionListener, IWorkbenchAction {

	private Logger logger = CTLogger.getInstance();
	private static final String NEW_RESOURCE_FILE_NAME = "NewResourceFileName.raml";
	private static final String SELECT_FILE_TEXT = "Select File..";
	private static final String RESOURCE_FILE_EXIST_MESSAGE = "File Already exist .Do you want to overwrite?";
	private static final String RESOURCE_FILE_IMPORT_MESSAGE = "File(s) Imported Successfully";
	private String[] files;

	public ImportResourceFileAction() {
		super(TOOLTIP_TEXT_IMPORT_RESOURCE_FILE.toString());
		this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
				Activator.PLUGIN_ID,
				IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_TEST_SUITE.toString()));
		this.setToolTipText(TOOLTIP_TEXT_IMPORT_RESOURCE_FILE.toString());
	}

	@Override
	public void run() {
		IWorkbenchPage page = PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getActivePage();
		RAMLView resourceView = (RAMLView) page.findView(RAMLView.ID);

		FileDialog fileDialog = new FileDialog(Display.getCurrent()
				.getActiveShell(), SWT.MULTI);
		String[] filterExtensions = new String[] { "*.raml;*.json;" };
		fileDialog.setText(SELECT_FILE_TEXT);
		fileDialog.setFileName(NEW_RESOURCE_FILE_NAME);
		fileDialog.setFilterExtensions(filterExtensions);
		String firstFile = fileDialog.open();
		if (firstFile != null) {
			files = fileDialog.getFileNames();

			logger.info("Response :" + files);
			String filterPath = fileDialog.getFilterPath();
			for (String fileName : files) {
				if (fileName != null) {
					File srcFile = new File(filterPath, fileName);
					File destFile = new File(UIConst.ROOT_PATH
							+ UIConst.RAML_PATH + "/" + srcFile.getName());
					if (!destFile.exists()) {
						try {
							FileUtils.copyFile(srcFile, destFile);
							showImportMessageBox();
						} catch (IOException e) {
							e.printStackTrace();
						}
					} else {
						int response = displayMessageBox(
								ACTIVE_WORKBENCH_WINDOW_SHELL,
								RESOURCE_FILE_EXIST_MESSAGE, EXIST_TEXT,
								SWT.YES | SWT.NO);
						logger.info("Response :" + response);
						if (response == SWT.YES) {
							try {
								FileUtils.copyFile(srcFile, destFile);
								showImportMessageBox();
							} catch (IOException e) {
								e.printStackTrace();
							}
						}
					}
					if (resourceView != null) {
						resourceView.refreshTree();
					}
					resourceView.setFocusCurrentSuite(destFile);
				}
			}
		}
	}

	@Override
	public void dispose() {

	}

	@Override
	public void selectionChanged(IWorkbenchPart part, ISelection selection) {

	}

	private void showImportMessageBox() {
		displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
				Integer.toString(files.length) + " "
						+ RESOURCE_FILE_IMPORT_MESSAGE, IMPORT_RESOURCE_TEXT,
				SWT.OK);
	}

}
