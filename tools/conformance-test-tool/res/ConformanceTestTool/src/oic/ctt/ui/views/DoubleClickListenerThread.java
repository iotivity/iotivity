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

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.FindReplaceDocumentAdapter;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ListViewer;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.texteditor.IDocumentProvider;
import org.slf4j.Logger;

import com.nitorcreations.robotframework.eclipseide.editors.RobotFrameworkTextfileEditor;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.multipages.ResourceEditor;
import oic.ctt.ui.multipages.ResourceEditorInput;
import oic.ctt.ui.multipages.TestPlanMultiPageEditor;
import oic.ctt.ui.multipages.TestSuiteMultiPageEditor;
import oic.ctt.ui.multipages.TestSuiteSpecPage;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.util.PopUpUtil.*;

public class DoubleClickListenerThread implements IDoubleClickListener {
	private IWorkbenchPage page;
	private Object parentClass;
	private Logger logger = CTLogger.getInstance();
	public static String resourceHref;

	public DoubleClickListenerThread(IWorkbenchPage workbenchPage, Object parent) {
		page = workbenchPage;
		parentClass = parent;
	}

	@Override
	public void doubleClick(DoubleClickEvent event) {

		if (parentClass instanceof ResourceView) {
			ListViewer viewer = (ListViewer) event.getViewer();
			IStructuredSelection selection = (IStructuredSelection) viewer
					.getSelection();

			if (selection.isEmpty())
				return;
			resourceHref = selection.getFirstElement().toString();
			IEditorInput idt = new ResourceEditorInput();
			try {
				UIConst.closeOpenedEditors(page);
				page.openEditor(idt, ResourceEditor.ID, false);
			} catch (PartInitException e) {
				throw new RuntimeException(e);
			}
			return;
		}
		TreeViewer viewer = (TreeViewer) event.getViewer();
		Object obj = ((IStructuredSelection) event.getSelection())
				.getFirstElement();
		if (viewer.getExpandedState(obj)) {
			viewer.collapseToLevel(obj, 1);
		} else {
			viewer.expandToLevel(obj, 1);
		}

		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		IWorkspaceRoot root = workspace.getRoot();
		String testPath = "";

		if (parentClass instanceof TestSuiteView) {
			testPath = UIConst.TESTSUITE_PATH;
		} else if (parentClass instanceof TestPlanView) {
			testPath = UIConst.TESTPLAN_PATH;
		} else if (parentClass instanceof TestReportView) {
			testPath = UIConst.TESTREPORT_PATH;
		} else if (parentClass instanceof TestPlanView) {
			testPath = UIConst.RAML_PATH;
		} else if (parentClass instanceof ResourceView) {
			testPath = UIConst.RAML_PATH;
		}

		IProject newProjectHandle = root.getProject(testPath);

		IStructuredSelection selection = (IStructuredSelection) event
				.getSelection();

		if (selection.getFirstElement() instanceof File) {
			File file = (File) selection.getFirstElement();

			String findName = null;

			try {
				findName = UIConst.getIFileFindName(file);
			} catch (Exception e) {
				logger.error("Throws exception when getting IFileFindName. Exception is: "
						+ e.getLocalizedMessage());
			}

			IFile ifile = newProjectHandle.getFile(findName);

			if (file.isFile()) {
				try {
					IDE.openEditor(page, ifile, true);
				} catch (PartInitException e) {
					e.printStackTrace();
				}
			} else if (file.isDirectory()) {
				if (!(parentClass instanceof TestReportView)) {
					try {
						IDE.openEditor(page, ifile, TestPlanMultiPageEditor.ID);
					} catch (PartInitException e) {
						e.printStackTrace();
					}
				}
			} else {
				if (parentClass instanceof TestReportView) {
					displayMessageBox(
							ACTIVE_WORKBENCH_WINDOW_SHELL,
							"Can't open the file. Please select the report file.",
							DIALOG_TITLE, SWT.ICON_WORKING | SWT.OK);

					logger.warn("Can't open the file. Please select the report file.");
				} else {
					logger.info("testcase click :" + file.getName());

					findName = UIConst.getIFileFindName(file.getParentFile());
					ifile = newProjectHandle.getFile(findName);
					IEditorPart editorpart = null;

					try {
						editorpart = IDE.openEditor(page, ifile, true);
					} catch (PartInitException e) {
						e.printStackTrace();
					}

					if (editorpart instanceof TestSuiteMultiPageEditor) {
						TestSuiteMultiPageEditor suiteEditor = (TestSuiteMultiPageEditor) editorpart;
						if (suiteEditor.getActivePageInstance() instanceof TestSuiteSpecPage) {
							TestSuiteSpecPage specpage = (TestSuiteSpecPage) suiteEditor
									.getActivePageInstance();
							String tcName = "";
							String tcFullName = file.getName();

							if (tcFullName.contains("]")) {
								tcName = tcFullName.split("\\]")[1].trim();
							} else {
								tcName = tcFullName;
							}
							specpage.setSelectionItem(tcName);
						} else if (suiteEditor.getActivePageInstance() == null) {
							if (suiteEditor.getActiveEditor() instanceof RobotFrameworkTextfileEditor) {
								doRobotFWActivity(file, suiteEditor);
							}
						}
					}
				}
			}
		} else if (selection.getFirstElement() instanceof String) {
			logger.debug("Selection.getFirstElement is String");
		}
	}

	/**
	 * Robot FW Text file Editor Activity
	 * 
	 * @param file
	 *            is test file
	 * @param suiteEditor
	 *            is TestSuite MultiPage Editor
	 */
	private void doRobotFWActivity(File file,
			TestSuiteMultiPageEditor suiteEditor) {
		RobotFrameworkTextfileEditor editor = (RobotFrameworkTextfileEditor) suiteEditor
				.getActiveEditor();
		IDocumentProvider provider = editor.getDocumentProvider();
		IDocument document = provider.getDocument(editor.getEditorInput());
		FindReplaceDocumentAdapter documentAdapter = new FindReplaceDocumentAdapter(
				document);
		try {
			IRegion findTextoffset = documentAdapter.find(0, file.getName(),
					true, false, false, false);
			int lineNumber = document.getLineOfOffset(findTextoffset
					.getOffset());
			int start = document.getLineOffset(lineNumber);
			editor.selectAndReveal(start, 0);
		} catch (BadLocationException e) {
			e.printStackTrace();
		}
	}
}
