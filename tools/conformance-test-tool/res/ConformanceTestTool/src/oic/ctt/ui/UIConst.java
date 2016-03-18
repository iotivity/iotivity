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

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import oic.ctt.ui.util.CTLogger;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.IFileEditorMapping;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.internal.registry.EditorDescriptor;
import org.eclipse.ui.internal.registry.EditorRegistry;
import org.eclipse.ui.internal.registry.FileEditorMapping;
import org.slf4j.Logger;

public class UIConst {
	private static Logger       logger                = CTLogger.getInstance();
	public static final String ROBOT_EDITOR_ID = "com.nitorcreations.robotframework.eclipseide.editors.RobotFrameworkTextfileEditor";
	public static final String WEB_BROWSER_ID = "org.eclipse.ui.browser.editor";
	public static final String PROJECT_PATH = Activator.class
			.getProtectionDomain().getCodeSource().getLocation().getPath();
	public static final String RESULT_PATH = "result";
	public static final String TESTREPORT_PATH = "testreport";
	public static final String TESTSUITE_PATH = "testsuite";
	public static final String TESTPLAN_PATH = "testplan";
	public static final String RAML_PATH = "ramls";
	public static final String TEMPLETE_PATH = "templete";
	public static final String REPORT_FILE_NAME = "/report.html";
	public static final String FLOWGRAPH_FILE_NAME = "FlowGraph.dat";
	public static final String AT_REST = "Ready";
	public static final String ROBOT_EXT = ".txt";
	public static final String RAML_EXT = ".raml";
	public static final String DEBUG_FILE_NAME = "debugLog.log";
	public static final String ROOT_PATH = Platform.getInstallLocation()
			.getURL().getPath();
	public static final String STATUS_NOT_TESTED = "Not Tested";
	public static final String STATUS_IN_PROGRESS = "In Progress";
	public static final String STATUS_DONE = "Done";
	public static final String STATUS_PASS = "Pass";
	public static final String STATUS_FAIL = "Fail";
	public static final String TOOLBAR_TEXT_START = " Start ";
	public static final String TOOLBAR_TEXT_STOP = " Stop ";
	public static final String TOOLBAR_TEXT_PAUSERESUME = " Pause / Resume ";
	public static final String REFRESH_ACTION_TEXT = "Refresh";
	public static final String SAVE_ACTION_TEXT = "SAVE RESOURCE";
	public static final String CREATE_RESOURCE_ACTION_TEXT = "Create Resource.";

	public static String getIFileFindName(File file) {
		String findName = "";
		File parent = null;

		while (true) {
			parent = file.getParentFile();
			if (parent != null) {
				findName = file.getName() + findName;
				if (parent.getName().equals(UIConst.TESTSUITE_PATH)
						|| parent.getName().equals(UIConst.TESTPLAN_PATH)
						|| parent.getName().equals(UIConst.TESTREPORT_PATH)) {
					break;
				}

			} else {
				findName = file.getName() + findName;
				break;
			}
			findName = "/" + findName;
			file = parent;
		}
		return findName;

	}

	public static MultiStatus createMultiStatus(String msg, Throwable t) {

		List<Status> childStatuses = new ArrayList<Status>();
		StackTraceElement[] stackTraces = Thread.currentThread()
				.getStackTrace();

		for (StackTraceElement stackTrace : stackTraces) {
			Status status = new Status(IStatus.ERROR, Activator.PLUGIN_ID,
					stackTrace.toString());
			childStatuses.add(status);
		}

		MultiStatus ms = new MultiStatus(Activator.PLUGIN_ID, IStatus.ERROR,
				childStatuses.toArray(new Status[] {}), t.toString(), t);
		return ms;
	}

	public static void createProject(String name) {

		IWorkspace workspace = ResourcesPlugin.getWorkspace();
		IWorkspaceRoot root = workspace.getRoot();
		IProject newProjectHandle = root.getProject(name);
		IPath targetPath = null;

		targetPath = new Path(ROOT_PATH + name);

		if (!newProjectHandle.exists()) {
			try {
				final IProjectDescription description = workspace
						.newProjectDescription(newProjectHandle.getName());
				description.setLocation(targetPath);
				newProjectHandle.create(description, null);
				newProjectHandle.open(null);
			} catch (CoreException e) {
				e.printStackTrace();
			}
		} else {
			logger.info(name + " project Handle exist, already.");
		}
	}

	@SuppressWarnings("restriction")
	public static void setAssociateEditorToFile(String strFileName,
			String strFileExt, String strEditorId) {
		if ((strFileExt.equals("")) || (strEditorId.equals(""))) {
			return;
		}

		EditorRegistry editorReg = (EditorRegistry) PlatformUI.getWorkbench()
				.getEditorRegistry();
		if (editorReg != null) {
			EditorDescriptor editor = (EditorDescriptor) editorReg
					.findEditor(strEditorId);
			FileEditorMapping mapping;
			if (strFileName.equals("")) {
				mapping = new FileEditorMapping(strFileExt);
			} else {
				mapping = new FileEditorMapping(strFileName, strFileExt);
			}
			mapping.addEditor(editor);
			mapping.setDefaultEditor(editor);
			IFileEditorMapping[] mappings = editorReg.getFileEditorMappings();
			FileEditorMapping[] newMappings = new FileEditorMapping[mappings.length + 1];
			for (int i = 0; i < mappings.length; i++) {
				newMappings[i] = (FileEditorMapping) mappings[i];
			}
			newMappings[mappings.length] = mapping;
			editorReg.setFileEditorMappings(newMappings);
		}
	}

	/**
	 * Close all opened editors in current active page
	 * 
	 * @param page
	 *            current active page
	 * @return none
	 */
	public static void closeOpenedEditors(IWorkbenchPage page) {
		IEditorReference[] editor = page.getEditorReferences();
		page.closeEditors(editor, false);
	}

	/**
	 * To get current active page
	 * 
	 * @return current active page
	 */
	public static IWorkbenchPage getActivePage() {
		IWorkbench workbench = PlatformUI.getWorkbench();
		IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();
		IWorkbenchPage page = window.getActivePage();
		return page;
	}
}
