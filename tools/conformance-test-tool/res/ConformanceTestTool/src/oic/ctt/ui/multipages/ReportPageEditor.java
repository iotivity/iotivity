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

import oic.ctt.ui.UIConst;
import static oic.ctt.ui.types.IDType.*;

import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.browser.CloseWindowListener;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.browser.ProgressEvent;
import org.eclipse.swt.browser.ProgressListener;
import org.eclipse.swt.browser.StatusTextEvent;
import org.eclipse.swt.browser.StatusTextListener;
import org.eclipse.swt.browser.WindowEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.MultiPageEditorPart;

/**
 * An example showing how to create a multi-page editor. This example has 3
 * pages:
 * <ul>
 * <li>page 0 contains a nested text editor.
 * <li>page 1 allows you to change the font used in page 2
 * <li>page 2 shows the words in page 0 in sorted order
 * </ul>
 */
public class ReportPageEditor extends MultiPageEditorPart implements
        IResourceChangeListener {

    public static final String ID   = REPORT_PAGE_EDITOR_ID.toString();

    Browser                    browser;
    private String             home = "";

    /**
     * Creates a multi-page editor example.
     */
    public ReportPageEditor() {
        super();
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    /**
     * Creates page 1 of the multi-page editor, which allows you to change the
     * font used in page 2.
     */
    void createPage() {

        Composite parent = new Composite(getContainer(), SWT.NONE);
        parent.setLayout(new FillLayout());

        // Create the web browser
        browser = new Browser(parent, SWT.BORDER);

        int index = addPage(parent);
        setPageText(index, "Test Report");
    }

    /**
     * Creates page 2 of the multi-page editor, which shows the sorted text.
     */

    /**
     * Creates the pages of the multi-page editor.
     */
    protected void createPages() {
        createPage();
        String reportPath = getEditorInput().toString().split("\\(")[1]
                .split("\\)")[0];
        String url = UIConst.ROOT_PATH + reportPath;
        setUrl(url);
    }

    /**
     * The <code>MultiPageEditorPart</code> implementation of this
     * <code>IWorkbenchPart</code> method disposes all nested editors.
     * Subclasses may extend.
     */
    public void dispose() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
        super.dispose();
    }

    /**
     * The <code>MultiPageEditorExample</code> implementation of this method
     * checks that the input is an instance of <code>IFileEditorInput</code>.
     */
    public void init(IEditorSite site, IEditorInput editorInput)
            throws PartInitException {
        if (!(editorInput instanceof IFileEditorInput)) {
            throw new PartInitException(
                    "Invalid Input: Must be IFileEditorInput");
        }
        super.init(site, editorInput);
    }

    /**
     * Calculates the contents of page 2 when the it is activated.
     */
    protected void pageChange(int newPageIndex) {
        super.pageChange(newPageIndex);
    }

    public void setUrl(String filename) {
        browser.setUrl(filename);
        home = filename;
    }

    /**
     * This class implements a CloseWindowListener for AdvancedBrowser
     */
    static class AdvancedCloseWindowListener implements CloseWindowListener {
        /**
         * Called when the parent window should be closed
         */
        public void close(WindowEvent event) {
            // Close the parent window
            ((Browser) event.widget).getShell().close();
        }
    }

    /**
     * This class implements a LocationListener for AdvancedBrowser
     */
    static class AdvancedLocationListener implements LocationListener {
        // The address text box to update
        private Text location;

        /**
         * Constructs an AdvancedLocationListener
         * 
         * @param text
         *            the address text box to update
         */
        public AdvancedLocationListener(Text text) {
            // Store the address box for updates
            location = text;
        }

        /**
         * Called before the location changes
         * 
         * @param event
         *            the event
         */
        public void changing(LocationEvent event) {
            // Show the location that's loading
            location.setText("Loading " + event.location + "...");
        }

        /**
         * Called after the location changes
         * 
         * @param event
         *            the event
         */
        public void changed(LocationEvent event) {
            // Show the loaded location
            location.setText(event.location);
        }
    }

    /**
     * This class implements a ProgressListener for AdvancedBrowser
     */
    static class AdvancedProgressListener implements ProgressListener {
        // The label on which to report progress
        private Label progress;

        /**
         * Constructs an AdvancedProgressListener
         * 
         * @param label
         *            the label on which to report progress
         */
        public AdvancedProgressListener(Label label) {
            // Store the label on which to report updates
            progress = label;
        }

        /**
         * Called when progress is made
         * 
         * @param event
         *            the event
         */
        public void changed(ProgressEvent event) {
            // Avoid divide-by-zero
            if (event.total != 0) {
                // Calculate a percentage and display it
                int percent = (int) (event.current / event.total);
                progress.setText(percent + "%");
            } else {
                // Since we can't calculate a percent, show confusion :-)
                progress.setText("???");
            }
        }

        /**
         * Called when load is complete
         * 
         * @param event
         *            the event
         */
        public void completed(ProgressEvent event) {
            // Reset to the "at rest" message
            progress.setText(UIConst.AT_REST);
        }
    }

    /**
     * This class implements a StatusTextListener for AdvancedBrowser
     */
    static class AdvancedStatusTextListener implements StatusTextListener {
        // The label on which to report status
        private Label status;

        /**
         * Constructs an AdvancedStatusTextListener
         * 
         * @param label
         *            the label on which to report status
         */
        public AdvancedStatusTextListener(Label label) {
            // Store the label on which to report status
            status = label;
        }

        /**
         * Called when the status changes
         * 
         * @param event
         *            the event
         */
        public void changed(StatusTextEvent event) {
            // Report the status
            status.setText(event.text);
        }
    }

    @Override
    public void resourceChanged(IResourceChangeEvent arg0) {

    }

    @Override
    public void doSave(IProgressMonitor arg0) {

    }

    @Override
    public void doSaveAs() {

    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }
}
