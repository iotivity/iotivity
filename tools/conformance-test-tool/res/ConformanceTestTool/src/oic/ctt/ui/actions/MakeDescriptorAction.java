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

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecuteResultHandler;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.ExecuteException;
import org.apache.commons.exec.PumpStreamHandler;
import org.apache.commons.io.output.ByteArrayOutputStream;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;

import oic.ctt.ui.util.ResourceUtil;
import static oic.ctt.ui.types.ToolTipTextType.TOOLTIP_TEXT_MAKE_DESCRIPTOR_FILE;
import static oic.ctt.ui.util.PopUpUtil.ACTIVE_WORKBENCH_WINDOW_SHELL;
import static oic.ctt.ui.util.PopUpUtil.RESOURCE_SAVE_TEXT;
import static oic.ctt.ui.util.PopUpUtil.displayMessageBox;
import static oic.ctt.ui.UIConst.ROOT_PATH;
import oic.ctt.ui.views.RAMLView;

public class MakeDescriptorAction extends Action implements ISelectionListener,
		IWorkbenchAction {

	private static final String DESCRIPTOR_CREATED_SUCCESSFULLY = "Descriptor created successfully";
	private static final String RAML_PARSER_SCRIPT_NAME = "/RAMLExecutor.sh";
	private static final String RAML_PARSER_SCRIPT_PATH = ROOT_PATH
			+ "RAMLParser";
	private String parserArgumnts;

	public MakeDescriptorAction() {
		super(TOOLTIP_TEXT_MAKE_DESCRIPTOR_FILE.toString(), AS_PUSH_BUTTON);
		this.setToolTipText(TOOLTIP_TEXT_MAKE_DESCRIPTOR_FILE.toString());
		this.setText("Make Descriptor");
	}

	private ArrayList<File> getSelectedRAMLFiles() {

		IWorkbenchPage page = PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getActivePage();
		RAMLView resourceView = (RAMLView) page.findView(RAMLView.ID);
		ArrayList<File> ramlFileList = new ArrayList<File>();
		Object[] items = resourceView.getSelectedItems();
		for (int i = 0; i < items.length; i++) {
			if (items[i] instanceof File) {
				File selectItems = (File) items[i];
				if (selectItems.exists()) {
					if (selectItems.isFile()) {
						ramlFileList.add(selectItems);
					}

				}
			}
		}
		return ramlFileList;

	}

	private void setParserArgumnts() {
		ArrayList<File> ramlFileList = getSelectedRAMLFiles();
	    if(ramlFileList.size() > 0) {
	    	parserArgumnts = "";
			for (Object file : ramlFileList.toArray()) {
				File selectFile = (File) file;
				parserArgumnts += selectFile.getName() + ",";
			}
	    }
	}

	public void run() {
		setParserArgumnts();
		CommandLine commandLine = new CommandLine(RAML_PARSER_SCRIPT_PATH
				+ RAML_PARSER_SCRIPT_NAME);
		commandLine.addArgument(parserArgumnts);
		executeRAMLParser(commandLine);
	}

	@Override
	public void dispose() {
	}

	@Override
	public void selectionChanged(IWorkbenchPart part, ISelection selection) {
	}

	private void executeRAMLParser(CommandLine commandLine) {
		DefaultExecutor executor = new DefaultExecutor();
		DefaultExecuteResultHandler resultHandler = new DefaultExecuteResultHandler();
		File scriptFile = new File(RAML_PARSER_SCRIPT_PATH);
		executor.setWorkingDirectory(scriptFile);
		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		PumpStreamHandler streamHandler = new PumpStreamHandler(baos);
		executor.setStreamHandler(streamHandler);
		try {
			executor.execute(commandLine, resultHandler);
			executor.setExitValue(1);
			displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
					DESCRIPTOR_CREATED_SUCCESSFULLY, RESOURCE_SAVE_TEXT, SWT.OK);
			resultHandler.waitFor();
			ResourceUtil.refreshResourceView();
		} catch (ExecuteException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}
