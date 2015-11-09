package oic.simulator.serviceprovider.view.dialogs;

import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.oic.simulator.server.SimulatorResource.Type;

public class MainPage extends WizardPage {

    private Button simpleBtn;
    private Button collectionBtn;
    private Button simpleFromRamlBtn;
    private Button collectionFromRamlBtn;
    private Button deviceBtn;
    private Text   description;

    public enum ResourceOption {
        SIMPLE, SIMPLE_FROM_RAML, COLLECTION, COLLECTION_FROM_RAML, DEVICE, NONE
    }

    private ResourceOption resOption;

    protected MainPage() {
        super("Main Page");
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.MAIN_PAGE_TITLE);
        setMessage(Constants.MAIN_PAGE_MESSAGE);
        Composite compContent = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        compContent.setLayout(gridLayout);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        compContent.setLayoutData(gd);

        simpleBtn = new Button(compContent, SWT.RADIO);
        simpleBtn.setText("Simple resource");

        simpleFromRamlBtn = new Button(compContent, SWT.RADIO);
        simpleFromRamlBtn.setText("Simple resource(From RAML)");

        collectionBtn = new Button(compContent, SWT.RADIO);
        collectionBtn.setText("Collection resource");

        collectionFromRamlBtn = new Button(compContent, SWT.RADIO);
        collectionFromRamlBtn.setText("Collection resource(From RAML)");

        deviceBtn = new Button(compContent, SWT.RADIO);
        deviceBtn.setText("Device");

        Label lbl = new Label(compContent, SWT.NULL);
        lbl.setText("Details:");
        gd = new GridData();
        gd.verticalIndent = 20;
        lbl.setLayoutData(gd);

        description = new Text(compContent, SWT.MULTI | SWT.READ_ONLY
                | SWT.BORDER | SWT.WRAP | SWT.V_SCROLL);
        description.setBackground(compContent.getBackground());
        description.setText("[Select an option to view more details]");
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        description.setLayoutData(gd);

        addUIListeners();

        setControl(compContent);
    }

    private void addUIListeners() {
        simpleBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                description
                        .setText("Create a simple resource by manually entering all the details given below.\n"
                                + "\t1. Basic resource details: URI, Name, Resource Types, etc.\n"
                                + "\t2. Options such as Observable, allowed request types, start/stop resource etc.\n"
                                + "\t3. Adding attributes.");
                resOption = ResourceOption.SIMPLE;
                getWizard().getContainer().updateButtons();
            }
        });

        simpleFromRamlBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                description
                        .setText("Create a simple resource from RAML configuration file.\n"
                                + "\t1. Resource details, attributes, and other properties will be read from RAML.\n"
                                + "\t2. Supports multi-instance creation.\n"
                                + "\t3. For single instance creation, allows editing the URI and Name of the resource.\n"
                                + "\t4. Allows to start or stop the resource(s).");
                resOption = ResourceOption.SIMPLE_FROM_RAML;
                getWizard().getContainer().updateButtons();
            }
        });

        collectionBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                description
                        .setText("Create a collection resource by manually entering all the details given below.\n"
                                + "\t1. Basic resource details: URI, Name, Resource Types, etc.\n"
                                + "\t2. Start/stop the resource.\n"
                                + "\t3. Adding existing simple resources to this collection.");
                resOption = ResourceOption.COLLECTION;
                getWizard().getContainer().updateButtons();
            }
        });

        collectionFromRamlBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                description
                        .setText("Create a collection resource from RAML configuration file.\n"
                                + "\t1. Resource details, attributes, and other properties will be read from RAML.\n"
                                + "\t2. Supports multi-instance creation.\n"
                                + "\t3. For single instance creation, allows editing the URI and Name of the resource.\n"
                                + "\t4. Allows to start or stop the resource(s).");
                resOption = ResourceOption.COLLECTION_FROM_RAML;
                getWizard().getContainer().updateButtons();
            }
        });

        deviceBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                description
                        .setText("Create a logical device that acts as a top-level logical classification.\n"
                                + "\t1. Takes a device name for identication.\n"
                                + "\t2. One or more single and collection resources can be added to it.");
                resOption = ResourceOption.DEVICE;
                getWizard().getContainer().updateButtons();
            }
        });
    }

    @Override
    public boolean canFlipToNextPage() {
        if (resOption != ResourceOption.NONE) {
            return true;
        }
        return false;
    }

    @Override
    public IWizardPage getPreviousPage() {
        return null;
    }

    @Override
    public IWizardPage getNextPage() {
        CreateResourceWizard createWizard = (CreateResourceWizard) getWizard();
        if (simpleBtn.getSelection()) {
            return createWizard.getSimpleResourceBasicDetailsPage();
        }
        if (simpleFromRamlBtn.getSelection()) {
            LoadRamlPage page = createWizard.getLoadRamlPage();
            page.initialSetup(Type.SINGLE);
            return page;
        }
        if (collectionBtn.getSelection()) {
            return createWizard.getCollectionResourceBasicDetailsPage();
        }
        if (collectionFromRamlBtn.getSelection()) {
            LoadRamlPage page = createWizard.getLoadRamlPage();
            page.initialSetup(Type.COLLECTION);
            return page;
        }
        if (deviceBtn.getSelection()) {
            DevicePage page = createWizard.getDevicePage();
            return page;
        }
        return null;
    }

    public ResourceOption getResourceOption() {
        return resOption;
    }
}
