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

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.utils.Constants;
import oic.simulator.serviceprovider.utils.Utility;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

public class SimpleResourceBasicDetailsPage extends WizardPage {

    private Text    resNameTxt;
    private Text    resUriTxt;
    private Text    resTypeTxt;
    private Button  observeBtn;

    private String  resName;
    private String  resURI;
    private String  resType;
    private boolean observable;

    protected SimpleResourceBasicDetailsPage() {
        super("Basic Details");
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.SIMPLE_RESOURCE_BASIC_DETAILS_PAGE_TITLE);
        setMessage(Constants.SIMPLE_RESOURCE_BASIC_DETAILS_PAGE_MESSAGE);

        Composite container = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        container.setLayout(gridLayout);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        container.setLayoutData(gd);

        Group resDetGrp = new Group(container, SWT.NONE);
        resDetGrp.setText("Resource Details");
        gridLayout = new GridLayout(4, false);
        resDetGrp.setLayout(gridLayout);
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        resDetGrp.setLayoutData(gd);

        Label resNameLbl = new Label(resDetGrp, SWT.NULL);
        resNameLbl.setText(Constants.RESOURCE_NAME);
        gd = new GridData();
        resNameLbl.setLayoutData(gd);

        resNameTxt = new Text(resDetGrp, SWT.BORDER);
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 3;
        resNameTxt.setLayoutData(gd);
        resNameTxt.setFocus();

        Label resUriLbl = new Label(resDetGrp, SWT.NULL);
        resUriLbl.setText(Constants.RESOURCE_URI);

        resUriTxt = new Text(resDetGrp, SWT.BORDER);
        gd = new GridData();
        gd.horizontalSpan = 3;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        resUriTxt.setLayoutData(gd);

        Label resTypeLbl = new Label(resDetGrp, SWT.NULL);
        resTypeLbl.setText(Constants.RESOURCE_TYPE);

        resTypeTxt = new Text(resDetGrp, SWT.BORDER);
        gd = new GridData();
        gd.horizontalSpan = 3;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        resTypeTxt.setLayoutData(gd);

        Group otherOptionsGrp = new Group(container, SWT.NONE);
        otherOptionsGrp.setText("Other options");
        gridLayout = new GridLayout();
        otherOptionsGrp.setLayout(gridLayout);
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        otherOptionsGrp.setLayoutData(gd);

        observeBtn = new Button(otherOptionsGrp, SWT.CHECK);
        observeBtn.setText("Observable");
        observeBtn.setSelection(true);
        observable = true;

        addUiListeners();

        setControl(container);
    }

    private void addUiListeners() {

        resUriTxt.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent arg0) {
                resURI = resUriTxt.getText();
                if (null == resURI) {
                    return;
                }

                getWizard().getContainer().updateButtons();
            }
        });

        resNameTxt.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent arg0) {
                resName = resNameTxt.getText();
                getWizard().getContainer().updateButtons();
            }
        });

        resTypeTxt.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent arg0) {
                resType = resTypeTxt.getText();
                getWizard().getContainer().updateButtons();
            }
        });

        observeBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                observable = observeBtn.getSelection();
            }
        });
    }

    @Override
    public boolean canFlipToNextPage() {
        if (null == resName || null == resURI || null == resType) {
            return false;
        }
        resName = resName.trim();
        resURI = resURI.trim();
        resType = resType.trim();
        if (resName.length() < 1 || resURI.length() < 1 || resType.length() < 1) {
            return false;
        }
        return true;
    }

    @Override
    public IWizardPage getNextPage() {
        if (!Utility.isUriValid(resURI)) {
            MessageDialog.openError(Display.getDefault().getActiveShell(),
                    "Invalid Resource URI.", Constants.INVALID_URI_MESSAGE);
            return null;
        }

        // Checking whether the uri is used by any other resource.
        if (Activator.getDefault().getResourceManager().isResourceExist(resURI)) {
            MessageDialog
                    .openError(getShell(), "Resource URI in use",
                            "Entered resource URI is in use. Please try a different one.");
            // TODO: Instead of MessageDialog, errors may be shown on wizard
            // itself.
            return null;
        }

        return ((CreateResourceWizard) getWizard())
                .getSimpleResourceAddAttributePage();
    }

    public String getResName() {
        return resName;
    }

    public String getResURI() {
        return resURI;
    }

    public String getResType() {
        return resType;
    }

    public boolean isObservable() {
        return observable;
    }
}
