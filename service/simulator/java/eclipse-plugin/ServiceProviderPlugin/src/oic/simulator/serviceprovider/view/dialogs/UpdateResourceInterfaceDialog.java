/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package oic.simulator.serviceprovider.view.dialogs;

import java.util.ArrayList;
import java.util.Map;
import java.util.Set;

import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TrayDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Shell;

/**
 * Dialog for starting and stopping the automatic verifications.
 */
public class UpdateResourceInterfaceDialog extends TrayDialog {
    private Map<String, Boolean> resourceIfStatus;

    public UpdateResourceInterfaceDialog(Shell shell,
            Map<String, Boolean> resourceIfStatus) {
        super(shell);
        this.resourceIfStatus = resourceIfStatus;
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);
        createAutomationGroup(composite);
        getShell().setText("Resource Interfaces");
        return composite;
    }

    /**
     * Dynamically creates a check-box list for changing the resource interface
     * selection.
     */
    private void createAutomationGroup(Composite parent) {
        Group group = new Group(parent, SWT.NONE);
        group.setLayout(new GridLayout());
        GridData gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.widthHint = 275;
        group.setLayoutData(gd);
        group.setText("Resource Interfaces");

        Set<String> keySet = resourceIfStatus.keySet();
        if (null == keySet) {
            return;
        }
        ArrayList<String> list = new ArrayList<String>(keySet);
        for (final String str : list) {
            final Button checkbox = new Button(group, SWT.CHECK);
            checkbox.setText(str);
            checkbox.setSelection(resourceIfStatus.get(str));

            // Disabling the check-box for baseline interface.
            if (str.contains(Constants.DEFAULT_SINGLE_RESOURCE_INTERFACE)) {
                checkbox.setEnabled(false);
            }

            checkbox.addSelectionListener(new SelectionAdapter() {

                @Override
                public void widgetSelected(SelectionEvent e) {
                    Button btn = (Button) e.getSource();
                    if (null == btn) {
                        return;
                    }
                    String btnText = btn.getText();
                    resourceIfStatus.put(btnText, btn.getSelection());
                }
            });
        }
    }

    @Override
    protected Button createButton(Composite parent, int id, String label,
            boolean defaultButton) {
        if (id == IDialogConstants.CANCEL_ID) {
            return null;
        }
        return super.createButton(parent, id, label, defaultButton);
    }

    @Override
    public boolean isHelpAvailable() {
        return false;
    }

    public Map<String, Boolean> getResourceInterfaceStatus() {
        return resourceIfStatus;
    }
}
