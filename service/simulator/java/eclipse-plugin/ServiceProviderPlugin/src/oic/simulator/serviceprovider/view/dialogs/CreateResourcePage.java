package oic.simulator.serviceprovider.view.dialogs;

import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
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

public class CreateResourcePage extends WizardPage {

    private Button stdResourceRbtn;
    private CCombo resourceTypeCmb;
    private Button cusResourceRbtn;
    private Text   locationTxt;
    private Button btnBrowse;
    private Text   noOfInstancesText;
    private Label  resourceTypeLbl;
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
        setTitle("Create Resource");
        setMessage("Select a standard resource or custom resource to be created");
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
        configGroup.setText("Configuration File Type");

        stdResourceRbtn = new Button(configGroup, SWT.RADIO);
        stdResourceRbtn.setText("Standard Configuration");

        Composite stdConfigComp = new Composite(configGroup, SWT.NONE);
        stdConfigComp.setLayout(new GridLayout(2, false));
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        stdConfigComp.setLayoutData(gd);

        resourceTypeLbl = new Label(stdConfigComp, SWT.NONE);
        resourceTypeLbl.setText("ResourceType:");
        resourceTypeLbl.setEnabled(false);

        resourceTypeCmb = new CCombo(stdConfigComp, SWT.READ_ONLY | SWT.BORDER);
        gd = new GridData();
        gd.widthHint = 150;
        resourceTypeCmb.setLayoutData(gd);
        resourceTypeCmb.setEnabled(false);

        cusResourceRbtn = new Button(configGroup, SWT.RADIO);
        cusResourceRbtn.setText("Custom Configuration");

        Composite cusConfigComp = new Composite(configGroup, SWT.NONE);
        cusConfigComp.setLayout(new GridLayout(3, false));
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        cusConfigComp.setLayoutData(gd);

        locationLbl = new Label(cusConfigComp, SWT.NONE);
        locationLbl.setText("Location:");
        locationLbl.setEnabled(false);

        locationTxt = new Text(cusConfigComp, SWT.BORDER);
        gd = new GridData();
        gd.minimumWidth = 300;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        locationTxt.setLayoutData(gd);
        locationTxt.setEnabled(false);

        btnBrowse = new Button(cusConfigComp, SWT.NONE);
        btnBrowse.setText("Browse");
        gd = new GridData();
        gd.widthHint = 80;
        gd.horizontalAlignment = SWT.FILL;
        btnBrowse.setLayoutData(gd);
        btnBrowse.setEnabled(false);

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

        populateDataInUI();

        addUIListeners();

        setControl(compContent);
    }

    private void populateDataInUI() {
        // Populate Resourcetype in Combo
        populateResourceTypeCombo();
    }

    private void populateResourceTypeCombo() {
        /*
         * List<String> configList; configList =
         * Activator.getDefault().getManager().getResourceConfigurationList();
         * if(null != configList) { Iterator<String> itr =
         * configList.iterator(); while(itr.hasNext()) {
         * resourceTypeCmb.add(itr.next()); } }
         */

        // TODO: Temporarily adding a resourceType for testing
        resourceTypeCmb.add("oic.light");

        // By default, selecting the first item in the resourceType combo
        if (resourceTypeCmb.getItemCount() > 0) {
            resourceTypeCmb.select(0);
            // TODO: Get the RAML configuration file path of the selected
            // resource
            // configFilePath =
            // Activator.getManager().getConfigFilePath(resourceTypeCmb.getItem(0));
        }
    }

    private void addUIListeners() {
        stdResourceRbtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {

                // Set the configFilePath to the first item in the combo
                if (resourceTypeCmb.getItemCount() > 0)
                    resourceTypeCmb.select(0);

                // TODO: Temporarily adding some random config file path
                // To be removed after implementing resource configuration
                configFilePath = "/Samplelight";

                setPageComplete(isSelectionDone());

                // Change the visibility of widgets
                resourceTypeLbl.setEnabled(true);
                resourceTypeCmb.setEnabled(true);
                locationLbl.setEnabled(false);
                locationTxt.setEnabled(false);
                btnBrowse.setEnabled(false);

            }
        });

        cusResourceRbtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setPageComplete(isSelectionDone());

                // Change the visibility of widgets
                locationLbl.setEnabled(true);
                locationTxt.setEnabled(true);
                btnBrowse.setEnabled(true);
                resourceTypeLbl.setEnabled(false);
                resourceTypeCmb.setEnabled(false);

            }
        });

        btnBrowse.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                FileDialog fileDialog = new FileDialog(PlatformUI
                        .getWorkbench().getDisplay().getActiveShell(), SWT.NONE);
                String configFileAbsolutePath = fileDialog.open();
                locationTxt.setText(configFileAbsolutePath);
            }
        });

        resourceTypeCmb.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                int index = resourceTypeCmb.getSelectionIndex();
                if (index >= 0) {
                    String selectedItem = resourceTypeCmb.getItem(index);
                    if (null != selectedItem) {
                        // TODO: Get the RAML configuration file path of the
                        // selected resource
                        // configFilePath =
                        // Activator.getDefault().getManager().getConfigFilePath(selectedItem);

                        setPageComplete(isSelectionDone());
                    }
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

    public boolean isSelectionDone() {
        boolean done = false;
        try {
            resourceCount = Integer.parseInt(noOfInstancesText.getText());
        } catch (Exception e) {
            resourceCount = -1;
        }
        if (cusResourceRbtn.getSelection()) {
            configFilePath = locationTxt.getText();
        }

        if (null != configFilePath && configFilePath.length() > 0
                && resourceCount >= 0) {
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
