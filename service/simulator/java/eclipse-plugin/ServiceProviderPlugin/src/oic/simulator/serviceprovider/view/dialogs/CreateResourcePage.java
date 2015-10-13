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

import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;

/**
 * This class shows UI for creating resources.
 */
public class CreateResourcePage extends WizardPage {

    private Text   locationTxt;
    private Button btnBrowse;
    private Text   noOfInstancesText;
    private Label  noOfInstancesLbl;
    private Label  locationLbl;

    private String configFilePath = null;
    private int    resourceCount;

    protected CreateResourcePage() {
        super("Create Resource");
        resourceCount = -1;
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.CREATE_PAGE_TITLE);
        setMessage(Constants.CREATE_PAGE_MESSAGE);
        Composite compContent = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout(1, false);
        compContent.setLayout(gridLayout);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        compContent.setLayoutData(gd);

        Group configGroup = new Group(compContent, SWT.NONE);
        gridLayout = new GridLayout(1, false);
        gridLayout.verticalSpacing = 10;
        gridLayout.marginTop = 5;
        configGroup.setLayout(gridLayout);
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        configGroup.setLayoutData(gd);
        configGroup.setText("Load RAML File");

        Composite cusConfigComp = new Composite(configGroup, SWT.NONE);
        cusConfigComp.setLayout(new GridLayout(3, false));
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        cusConfigComp.setLayoutData(gd);

        locationLbl = new Label(cusConfigComp, SWT.NONE);
        locationLbl.setText("Location:");

        locationTxt = new Text(cusConfigComp, SWT.BORDER);
        gd = new GridData();
        gd.minimumWidth = 300;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        locationTxt.setLayoutData(gd);

        btnBrowse = new Button(cusConfigComp, SWT.NONE);
        btnBrowse.setText("Browse");
        gd = new GridData();
        gd.widthHint = 80;
        gd.horizontalAlignment = SWT.FILL;
        btnBrowse.setLayoutData(gd);
        btnBrowse.setFocus();

        Composite instanceComp = new Composite(compContent, SWT.NONE);
        instanceComp.setLayout(new GridLayout(2, false));
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        instanceComp.setLayoutData(gd);

        noOfInstancesLbl = new Label(instanceComp, SWT.NONE);
        noOfInstancesLbl.setText("Number of Instances");

        noOfInstancesText = new Text(instanceComp, SWT.BORDER);
        gd = new GridData();
        gd.widthHint = 50;
        gd.horizontalAlignment = SWT.FILL;
        noOfInstancesText.setLayoutData(gd);

        // Adding the default value. It can be changed by the user.
        noOfInstancesText.setText("1");

        addUIListeners();

        setControl(compContent);
    }

    private void addUIListeners() {

        btnBrowse.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                FileDialog fileDialog = new FileDialog(PlatformUI
                        .getWorkbench().getDisplay().getActiveShell(), SWT.NONE);
                fileDialog
                        .setFilterExtensions(Constants.BROWSE_RAML_FILTER_EXTENSIONS);
                String configFileAbsolutePath = fileDialog.open();
                if (null != configFileAbsolutePath) {
                    locationTxt.setText(configFileAbsolutePath);
                }
            }
        });

        locationTxt.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent e) {
                configFilePath = locationTxt.getText();
                setPageComplete(isSelectionDone());
            }
        });

        noOfInstancesText.addListener(SWT.Verify, new Listener() {
            @Override
            public void handleEvent(Event e) {
                String string = e.text;
                char[] chars = new char[string.length()];
                string.getChars(0, chars.length, chars, 0);
                for (int i = 0; i < chars.length; i++) {
                    if (!('0' <= chars[i] && chars[i] <= '9')) {
                        e.doit = false;
                        return;
                    }
                }
            }
        });

        noOfInstancesText.addKeyListener(new KeyAdapter() {
            @Override
            public void keyReleased(KeyEvent e) {
                setPageComplete(isSelectionDone());
            }
        });
    }

    private boolean isSelectionDone() {
        boolean done = false;
        try {
            resourceCount = Integer.parseInt(noOfInstancesText.getText());
        } catch (Exception e) {
            resourceCount = -1;
        }

        if (null != configFilePath && configFilePath.length() > 0
                && resourceCount >= 1) {
            done = true;
        }
        return done;
    }

    public String getConfigFilePath() {
        return configFilePath;
    }

    public int getResourceCount() {
        return resourceCount;
    }
}