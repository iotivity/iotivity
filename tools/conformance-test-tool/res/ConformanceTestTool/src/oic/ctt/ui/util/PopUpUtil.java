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
package oic.ctt.ui.util;

import oic.ctt.ui.UIConst;

import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

public class PopUpUtil {

	public static final String DIALOG_TITLE = "Information";
	private static final String SELECT_PLAN_DIRECTORY_TEXT = "Select Plan Directory..";
	public static final String ERROR_TEXT = "ERROR";
	public static final String EXIST_TEXT = "EXIST";
	public static final String IMPORT_RESOURCE_TEXT = "RESOURCE FILE IMPORT";
	public static final String RESOURCE_SAVE_TEXT = " RESOURCE SAVE";
	public static final String DIALOG_BUTTON_LABEL_DELETE = "Delete";
	public static final String DIALOG_BUTTON_LABEL_CANCEL = "Cancel";
	public static final String DIALOG_BUTTON_LABEL_OK = "OK";
	public static final String RAML_FILE_DELETION_TEXT = "DELETE RAML FILE";
	public static final String RAML_FILES_DELETION_CONFIRMATION_MESSAGE  = "Are you sure you want to delete the selected RAML files?\n\n";
	public static final String RAML_FILES_DELETION_MESSAGE = "Files Deleted Successfully";
	public static final Shell ACTIVE_WORKBENCH_WINDOW_SHELL = PlatformUI
			.getWorkbench().getActiveWorkbenchWindow().getShell();
	public static final Shell ACTIVE_SHELL = Display.getDefault()
			.getActiveShell();

	/**
	 * This method displays a message box
	 * 
	 * @param shell
	 *            currently active Shell object
	 * @param message
	 *            message to be set
	 * @param text
	 *            text to be set
	 * @param style
	 *            message box style to be set
	 */

	public static int displayMessageBox(Shell shell, String message,
			String text, int style) {
		MessageBox box = createMessageBox(shell, message, text, style);
		int response = openMessageBox(box);
		return response;
	}

	/**
	 * This method creates a message box
	 * 
	 * @param shell
	 *            currently active Shell object
	 * @param message
	 *            message to be set
	 * @param text
	 *            text to be set
	 * @return box a MessageBox object
	 */
	public static MessageBox createMessageBox(Shell shell, String message,
			String text, int style) {
		MessageBox box = new MessageBox(shell, style);
		box.setMessage(message);
		box.setText(text);
		return box;
	}

	/**
	 * This method open a message box
	 * 
	 * @param messageBox
	 *            already created message box object
	 */
	public static int openMessageBox(MessageBox messageBox) {
		int response = messageBox.open();
		return response;
	}

	/**
	 * This method shows a directory dialog
	 * 
	 * @param shell
	 *            currently active Shell object
	 * @param text
	 *            text to be set
	 * @param path
	 *            the filter path
	 * @return dirDialog a DirectoryDialog object
	 */
	public static String displayDirectoryDialog(Shell shell, String text,
			String path) {
		DirectoryDialog dirDialog = createDirectoryDialog(shell,
				SELECT_PLAN_DIRECTORY_TEXT, UIConst.PROJECT_PATH);
		return openDirectoryDialog(dirDialog);
	}

	/**
	 * This method creates a directory dialog
	 * 
	 * @param shell
	 *            currently active shell object
	 * @param text
	 *            text to be set
	 * @param path
	 *            the filter path
	 * @return dirDialog a DirectoryDialog object
	 */
	public static DirectoryDialog createDirectoryDialog(Shell shell,
			String text, String path) {
		DirectoryDialog dirDialog = new DirectoryDialog(
				ACTIVE_WORKBENCH_WINDOW_SHELL, SWT.MULTI);
		dirDialog.setText(text);
		dirDialog.setFilterPath(path);
		return dirDialog;
	}

	/**
	 * This method open a directory dialog
	 * 
	 * @param dirDialog
	 *            a DirectoryDialog object
	 * @return the directory name
	 */
	public static String openDirectoryDialog(DirectoryDialog dirDialog) {
		String dirName = dirDialog.open();
		return dirName;
	}

	/**
	 * This method displays a message dialog
	 * 
	 * @param shell
	 *            currently active shell object previously created message
	 *            dialog object
	 * @param dialogTitle
	 *            title to be displayed
	 * @param dialogMessage
	 *            message to be displayed
	 * @param dialogImageType
	 *            which type image to be displayed
	 * @param dialogButtonLabels
	 *            labels to be displayed
	 */
	public static void displayMessageDialog(Shell shell, String dialogTitle,
			String dialogMessage, int dialogImageType,
			String[] dialogButtonLabels) {
		MessageDialog messageDialog = createMessageDialog(shell, dialogTitle,
				dialogMessage, dialogImageType, dialogButtonLabels);
		openMessageDialog(messageDialog);
	}

	/**
	 * This method creates a message dialog
	 * 
	 * @param shell
	 *            currently active shell object previously created message
	 *            dialog object
	 * @param dialogTitle
	 *            title to be displayed
	 * @param dialogMessage
	 *            message to be displayed
	 * @param dialogImageType
	 *            which type image to be displayed
	 * @param dialogButtonLabels
	 *            labels to be displayed
	 * @return messageDialog a MessageDialog object
	 */
	public static MessageDialog createMessageDialog(Shell shell,
			String dialogTitle, String dialogMessage, int dialogImageType,
			String[] dialogButtonLabels) {
		MessageDialog messageDialog = new MessageDialog(shell, dialogTitle,
				null, dialogMessage, dialogImageType, dialogButtonLabels, 0);
		return messageDialog;
	}

	/**
	 * This method open a message dialog that is already created
	 * 
	 * @param messageDialog
	 *            previously created message dialog object
	 * @param dialogTitle
	 *            title to be displayed in dialog
	 * @param message
	 */
	public static void openMessageDialog(MessageDialog messageDialog) {
		messageDialog.open();
	}

	/**
	 * This method display error dialog
	 * 
	 * @param e
	 *            exception object
	 * @param dialogTitle
	 *            title to be displayed in dialog
	 * @param message
	 *            message to be displayed in dialog
	 */
	public static void displayErrorDialog(Exception e, String dialogTitle,
			String message) {
		MultiStatus status = UIConst.createMultiStatus(e.getLocalizedMessage(),
				e);
		ErrorDialog.openError(ACTIVE_SHELL, dialogTitle, message, status);
		e.printStackTrace();
	}

}
