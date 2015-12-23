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

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.CTLogger;
import oic.ctt.ui.util.TestCaseParser;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.forms.editor.FormEditor;
import org.slf4j.Logger;

/**
 * An example showing how to create a multi-page editor. This example has 3
 * pages:
 * <ul>
 * <li>page 0 contains a nested text editor.
 * <li>page 1 allows you to change the font used in page 2
 * <li>page 2 shows the words in page 0 in sorted order
 * </ul>
 */
public class TestPlanMultiPageEditor extends FormEditor implements
        ISelectionListener {

    public static final String   ID     = TEST_PLAN_MULTI_PAGE_EDITOR_ID
                                                .toString();
    private Logger               logger = CTLogger.getInstance();

    public TextEditor            editor;
    // public TestPlanSpecPage testspecpage;
    public static TestCaseParser parser;

    /**
     * Creates a multi-page editor example.
     */
    public TestPlanMultiPageEditor() {
        super();
    }

    /**
     * Creates the pages of the multi-page editor.
     */

    protected void createPages() {
        FileInputStream fileInputStream = null;
        try {
            try {
                if (getPageCount() > 0) {
                    removePage(0);
                }
                TestPlanSpecPage testspecpage = new TestPlanSpecPage(this,
                        getEditorInput().getName());
                fileInputStream = new FileInputStream(
                        UIConst.PROJECT_PATH
                                + IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SETTINGS_ICON
                                        .toString());
                int index = addPage(testspecpage);
                setPageText(index, "Test Plan - Specification");
                setPageImage(index, new Image(null, fileInputStream));
                setPartName(getEditorInput().getName());
            } catch (PartInitException | FileNotFoundException e) {
                e.printStackTrace();
            }

            if (fileInputStream != null) {
                fileInputStream.close();
            }
        } catch (IOException e1) {
            e1.printStackTrace();
        }
    }

    /**
     * The <code>MultiPageEditorPart</code> implementation of this
     * <code>IWorkbenchPart</code> method disposes all nested editors.
     * Subclasses may extend.
     */
    public void dispose() {
        super.dispose();
    }

    /**
     * Saves the multi-page editor's document.
     */
    public void doSave(IProgressMonitor monitor) {
    }

    /**
     * Saves the multi-page editor's document as another file. Also updates the
     * text for page 0's tab, and updates this multi-page editor's input to
     * correspond to the nested editor's.
     */
    public void doSaveAs() {
    }

    public void gotoMarker(IMarker marker) {
    }

    public boolean isSaveAsAllowed() {
        return true;
    }

    @Override
    protected void addPages() {
    }

    @Override
    public void selectionChanged(IWorkbenchPart paramIWorkbenchPart,
            ISelection paramISelection) {
        logger.info("tab changed");
    }
}
