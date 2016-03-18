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
import java.util.ArrayList;

import oic.ctt.ui.UIConst;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.util.PopUpUtil.*;
import oic.ctt.ui.views.TestPlanView;
import oic.ctt.ui.views.TestReportView;
import oic.ctt.ui.views.TestSuiteView;
import oic.ctt.ui.views.RAMLView;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.slf4j.Logger;

public class DeleteAction extends Action implements ISelectionListener,
		IWorkbenchAction {
	private Logger logger = CTLogger.getInstance();
	private static final String NOT_SELECTED_ITEM_MESSAGE = "Not selected item.\n\n";
	private static final String ITEMS_SELECTION_MESSAGE = " item(s) selected.";
	private static final String TEST_ITEM_DELETION_DIALOG_WARNING_MESSAGE = "Not selected directory.\n\n";
	private static final String TEST_ITEM_DELETION_DIALOG_MESSAGE = "Are you sure you want to delete Test Item?\n\n";
	private static final String TEST_SUITE_DELETION_DIALOG_MESSAGE = "Delete Test Suite";
	private static final String TEST_ITEM_DELETION_DIALOG_TITLE = "Delete Test Item";
	public String category;
	private ArrayList<File> dirList;
	private ArrayList<File> fileList;
	private String list;
	private int count;
	private TestSuiteView testSuiteView;

	public DeleteAction(String value) {
		super(TOOLTIP_TEXT_DELETE_TEST_SUITE.toString());
		category = value;
		this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
				PLUGIN_ID.toString(),
				IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_DELETE_TEST_SUITE.toString()));
		this.setToolTipText(TOOLTIP_TEXT_DELETE_TEST_SUITE.toString());
	}

	@Override
	public void dispose() {
	}

	@Override
	public void selectionChanged(IWorkbenchPart part, ISelection selection) {
	}

	public void run() {

		IWorkbenchPage page = PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getActivePage();
		testSuiteView = (TestSuiteView) page.findView(TEST_SUITE_VIEW_ID
				.toString());
		TestPlanView testPlanView = (TestPlanView) page
				.findView(TEST_PLAN_VIEW_ID.toString());
		 TestReportView testReportView = (TestReportView) page
		 .findView(TEST_REPORT_VIEW_ID.toString());
		RAMLView ramlView = (RAMLView) page.findView(RAMLView.ID);

		dirList = new ArrayList<File>();
		fileList = new ArrayList<File>();
		list = "";
		count = 0;

		if (category.equals(UIConst.TESTSUITE_PATH) && testSuiteView != null) {
			deleteTestSuite(testSuiteView);
		} else if (category.equals(UIConst.TESTPLAN_PATH)
				|| category.equals(UIConst.TESTREPORT_PATH)
				|| category.equals(UIConst.RAML_PATH)) {
			IStructuredSelection selection = null;
			if (category.equals(UIConst.TESTPLAN_PATH) && testPlanView != null) {
				selection = (IStructuredSelection) testPlanView
						.getSelectionTree();
			}
			 else if (category.equals(UIConst.TESTREPORT_PATH)
			 && testReportView != null) {
			 selection = (IStructuredSelection) testReportView
			 .getSelectionTree();
			 }
			else if (category.equals(UIConst.RAML_PATH) && ramlView != null) {
				deleteResourceFiles(ramlView);
				return;
			}
			if (selection != null) {
				addSelectedItemToDirList(selection);

				if (dirList.size() > 0) {
					MessageDialog dialog = createMessageDialog(null,
							TEST_ITEM_DELETION_DIALOG_TITLE,
							TEST_ITEM_DELETION_DIALOG_MESSAGE + list + "\n"
									+ dirList.size() + ITEMS_SELECTION_MESSAGE,
							MessageDialog.QUESTION, new String[] {
									DIALOG_BUTTON_LABEL_DELETE,
									DIALOG_BUTTON_LABEL_CANCEL });
					int result = dialog.open();
					if (result == 0) {
						deleteDirectoryFromList(dirList);
						closeOpenedEditors();
					}
					if (category.equals(UIConst.TESTSUITE_PATH)
							&& testSuiteView != null) {
						testSuiteView.refreshTree();
					} else if (category.equals(UIConst.TESTPLAN_PATH)
							&& testPlanView != null) {
						testPlanView.refreshTree();
					}
					 else if (category.equals(UIConst.TESTREPORT_PATH)
					 && testReportView != null) {
					 testReportView.refreshTree();
					 }
					else if (category.equals(UIConst.RAML_PATH)
							&& ramlView != null) {
						ramlView.refreshTree();
					}
				} else {
					displayMessageDialog(
							null,
							TEST_ITEM_DELETION_DIALOG_TITLE,
							TEST_ITEM_DELETION_DIALOG_WARNING_MESSAGE
									+ dirList.size() + ITEMS_SELECTION_MESSAGE,
							MessageDialog.QUESTION,
							new String[] { DIALOG_BUTTON_LABEL_OK });
				}
			} else {
				logger.debug("Selection is null.");
			}
		}
	}

	/**
	 * This method adds selected file to directory list
	 * 
	 * @param selection
	 *            this is selected item
	 * @return nothing
	 */
	private void addSelectedItemToDirList(IStructuredSelection selection) {
		for (Object dir : selection.toList()) {
			if (dir instanceof File) {
				File dirPath = (File) dir;
				if (dirPath.exists()) {
					if (dirPath.isDirectory()) {
						if (count < 20) {
							list += dirPath.getName() + "\n";
						} else if (count == 20) {
							list += "........................\n";
						}
						dirList.add((File) dir);
						count++;
					}
				}
			}
		}
	}

	/**
	 * This method delete selected test suites
	 * 
	 * @param testSuiteView
	 *            this is the test suite to be deleted
	 */
	private void deleteTestSuite(TestSuiteView testSuiteView) {
		Object[] items = testSuiteView.getSelectedItems();
		addSelecTedItemsToFileAndDirectoryList(items);
		if (dirList.size() + fileList.size() > 0) {
			MessageDialog dialog = createMessageDialog(
					null,
					TEST_SUITE_DELETION_DIALOG_MESSAGE,
					TEST_ITEM_DELETION_DIALOG_MESSAGE + list + "\n"
							+ (dirList.size() + fileList.size())
							+ ITEMS_SELECTION_MESSAGE, MessageDialog.QUESTION,
					new String[] { DIALOG_BUTTON_LABEL_DELETE,
							DIALOG_BUTTON_LABEL_CANCEL });
			int result = dialog.open();
			if (result == 0) {
				deleteFilesFromFileList(fileList);
				deleteDirectoryFromList(dirList);
				closeOpenedEditors();
			}
			if (category.equals(UIConst.TESTSUITE_PATH)) {
				testSuiteView.setAllUnChecked();
				testSuiteView.refreshTree();
			}
		} else {
			displayMessageDialog(null, TEST_ITEM_DELETION_DIALOG_TITLE,
					NOT_SELECTED_ITEM_MESSAGE + dirList.size()
							+ ITEMS_SELECTION_MESSAGE, MessageDialog.WARNING,
					new String[] { DIALOG_BUTTON_LABEL_OK });
		}
	}

	/**
	 * This method find the opened editors and close all
	 * 
	 * @param nothing
	 * @return nothing
	 */
	private void closeOpenedEditors() {
		IWorkbenchPage[] pages = PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getPages();
		for (int i = 0; i < pages.length; i++) {
			IEditorReference[] editors = pages[i].getEditorReferences();
			for (int j = 0; j < editors.length; j++) {
				try {
					File file = ((FileEditorInput) editors[j].getEditorInput())
							.getPath().toFile();
					logger.info("File absolute path " + file.getAbsolutePath());
					if (file.exists() == false) {
						pages[i].closeEditor(editors[j].getEditor(true), true);
					}
				} catch (PartInitException e) {
					e.printStackTrace();
				}
			}
		}
	}

	/**
	 * This method delete directory from list
	 * 
	 * @param dirList
	 *            directory list to be deleted
	 * @return nothing
	 */
	private void deleteDirectoryFromList(ArrayList<File> dirList) {
		for (Object dir : dirList.toArray()) {
			File delDir = (File) dir;
			try {
				FileUtils.deleteDirectory(delDir);
				logger.info("Delete Test Suite Directory : " + delDir.getPath());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Delete all files from a file list
	 * 
	 * @param fileList
	 *            this is the file list to be deleted
	 * @param nothing
	 */
	private void deleteFilesFromFileList(ArrayList<File> fileList) {

		for (Object file : fileList.toArray()) {
			File delFile = (File) file;
			try {
				FileUtils.forceDelete(delFile);
				logger.info("Delete Test Suite File : " + delFile.getPath());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Add selected items name to a file and directory list according to item
	 * type
	 * 
	 * @param items
	 *            these are the items to be appended
	 * @return nothing
	 */
	private void addSelecTedItemsToFileAndDirectoryList(Object[] items) {
		for (int i = 0; i < items.length; i++) {
			if (items[i] instanceof File) {
				File selectItems = (File) items[i];
				if (selectItems.exists()) {
					if (selectItems.isFile()) {
						addSelectedItemToFileList(selectItems);
					}
					if (selectItems.isDirectory()) {
						addSelectedItemToDirectoryList(selectItems);
					}
				}
			}
		}
	}

	/**
	 * Add selected items name to a directory list
	 * 
	 * @param selectedItem
	 *            this is the item to be appended
	 * @return nothing
	 */
	private void addSelectedItemToDirectoryList(File selectedItem) {
		if (testSuiteView.getChildrenChecked(selectedItem)) {
			if (count < 20) {
				list += selectedItem.getName() + "\n";
			} else if (count == 20) {
				list += "........................\n";
			}
			dirList.add(selectedItem);
			count++;
		}
	}

	/**
	 * Add selected items name to a file list
	 * 
	 * @param selectedItem
	 *            this is the item to be appended
	 * @return nothing
	 */
	private void addSelectedItemToFileList(File selectedItem) {
		if (count < 20) {
			list += selectedItem.getName() + "\n";
		} else if (count == 20) {
			list += "........................\n";
		}
		fileList.add(selectedItem);
		count++;
	}

	/**
	 * Delete selected resource files and refresh resource view
	 * 
	 * @param ramlView
	 *            view to be refreshed
	 * @return nothing
	 */
	private void deleteResourceFiles(RAMLView ramlView) {
		Object[] items = ramlView.getSelectedItems();
		addSelecTedItemsToFileAndDirectoryList(items);
		int result = displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
						 RAML_FILES_DELETION_CONFIRMATION_MESSAGE, RAML_FILE_DELETION_TEXT,
				SWT.YES | SWT.NO);
		if (result == SWT.YES) {
			deleteFilesFromFileList(fileList);
			displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
					Integer.toString(items.length) + " "
							+ RAML_FILES_DELETION_MESSAGE, RAML_FILE_DELETION_TEXT,
					SWT.OK);
			ramlView.refreshTree();
		}
		
	}
}
