package oic.simulator.serviceprovider.view.dialogs;

import java.util.HashSet;
import java.util.Set;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.utils.Constants;
import oic.simulator.serviceprovider.utils.Utility;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.window.Window;
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
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Text;

public class CollectionResourceBasicDetailsPage extends WizardPage {

    private Text        resNameTxt;
    private Text        resUriTxt;
    private List        resTypeslist;
    private Button      addToListBtn;
    private Button      remFromListBtn;
    private Button      observeBtn;
    // private Button startBtn;

    private String      resName;
    private String      resURI;
    private Set<String> resTypes;
    // private boolean start;
    private boolean     observable;
    private boolean     anyResourceExist;

    protected CollectionResourceBasicDetailsPage() {
        super("Basic Details");
        resTypes = new HashSet<String>();
        anyResourceExist = Activator.getDefault().getResourceManager()
                .isAnyResourceExist();
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.COLLECTION_RESOURCE_BASIC_DETAILS_PAGE_TITLE);
        setMessage(Constants.COLLECTION_RESOURCE_BASIC_DETAILS_PAGE_MESSAGE);

        Composite container = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        container.setLayout(gridLayout);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        container.setLayoutData(gd);

        Group resDetGrp = new Group(container, SWT.NONE);
        resDetGrp.setText("Resource Details");
        gridLayout = new GridLayout(4, false);
        resDetGrp.setLayout(gridLayout);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        resDetGrp.setLayoutData(gd);

        Label resNameLbl = new Label(resDetGrp, SWT.NULL);
        resNameLbl.setText(Constants.RESOURCE_NAME);
        gd = new GridData();
        resNameLbl.setLayoutData(gd);

        resNameTxt = new Text(resDetGrp, SWT.BORDER);
        gd = new GridData();
        // gd.widthHint = 300;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 3;
        resNameTxt.setLayoutData(gd);
        resNameTxt.setFocus();

        Label resUriLbl = new Label(resDetGrp, SWT.NULL);
        resUriLbl.setText(Constants.RESOURCE_URI);

        resUriTxt = new Text(resDetGrp, SWT.BORDER);
        gd = new GridData();
        // gd.widthHint = 300;
        gd.horizontalSpan = 3;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        resUriTxt.setLayoutData(gd);
        /*
         * Composite resTypesComp = new Composite(compContent, SWT.NONE);
         * gridLayout = new GridLayout(3, false);
         * resTypesComp.setLayout(gridLayout); gd = new GridData();
         * gd.grabExcessHorizontalSpace = true; gd.horizontalAlignment =
         * SWT.FILL; resTypesComp.setLayoutData(gd);
         */

        Label resTypesLbl = new Label(resDetGrp, SWT.NULL);
        resTypesLbl.setText(Constants.RESOURCE_TYPES);

        resTypeslist = new List(resDetGrp, SWT.BORDER | SWT.MULTI
                | SWT.V_SCROLL);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.horizontalSpan = 2;
        gd.heightHint = 100;
        /* gd.widthHint = 200; */
        resTypeslist.setLayoutData(gd);
        // resTypeslist.setBackground(new Color());

        Composite resTypesActionsComp = new Composite(resDetGrp, SWT.NONE);
        gridLayout = new GridLayout();
        resTypesActionsComp.setLayout(gridLayout);
        gd = new GridData();
        gd.verticalAlignment = SWT.TOP;
        /*
         * gd.grabExcessHorizontalSpace = true; gd.horizontalAlignment =
         * SWT.FILL;
         */
        resTypesActionsComp.setLayoutData(gd);

        addToListBtn = new Button(resTypesActionsComp, SWT.PUSH);
        addToListBtn.setText("Add");
        gd = new GridData();
        gd.widthHint = 70;
        addToListBtn.setLayoutData(gd);

        remFromListBtn = new Button(resTypesActionsComp, SWT.PUSH);
        remFromListBtn.setText("Remove");
        gd = new GridData();
        gd.widthHint = 70;
        remFromListBtn.setLayoutData(gd);
        remFromListBtn.setEnabled(false);

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

        /*
         * startBtn = new Button(otherOptionsGrp, SWT.CHECK);
         * startBtn.setText("Start the resource immediately after creation");
         * startBtn.setSelection(true);
         */

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

                if (anyResourceExist)
                    getWizard().getContainer().updateButtons();
                else
                    setPageComplete(isSelectionDone());
            }
        });

        resNameTxt.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent arg0) {
                resName = resNameTxt.getText();
                if (null == resName) {
                    return;
                }
                if (anyResourceExist)
                    getWizard().getContainer().updateButtons();
                else
                    setPageComplete(isSelectionDone());
            }
        });

        addToListBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                SingleTextInputDialog dialog = new SingleTextInputDialog(
                        getShell(), "Add Resource Type", "Resource Type");
                if (dialog.open() == Window.OK) {
                    System.out.println(dialog.getValue());
                    String resType = dialog.getValue();
                    // Duplicate check.
                    if (resTypes.contains(resType)) {
                        MessageDialog.openError(Display.getDefault()
                                .getActiveShell(), "Duplicate value",
                                "value already exist.");
                        return;
                    }
                    resTypeslist.add(resType);
                    resTypeslist.deselectAll();
                    resTypeslist.select(resTypeslist.getItemCount() - 1);
                    resTypeslist.showSelection();
                    remFromListBtn.setEnabled(true);
                    resTypes.add(resType);
                    if (anyResourceExist)
                        getWizard().getContainer().updateButtons();
                    else
                        setPageComplete(isSelectionDone());
                }
            }
        });

        remFromListBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                int[] selection = resTypeslist.getSelectionIndices();
                String[] selectionStr = resTypeslist.getSelection();
                if (null != selection && selection.length > 0) {
                    resTypeslist.remove(selection);
                    for (String selected : selectionStr) {
                        resTypes.remove(selected);
                    }
                }
                changeVisibility();
                if (anyResourceExist)
                    getWizard().getContainer().updateButtons();
                else
                    setPageComplete(isSelectionDone());
            }
        });

        resTypeslist.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                changeVisibility();
            }
        });

        observeBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                observable = observeBtn.getSelection();
            }
        });

        /*
         * startBtn.addSelectionListener(new SelectionAdapter() {
         * 
         * @Override public void widgetSelected(SelectionEvent e) { start =
         * startBtn.getSelection(); } });
         */
    }

    private void changeVisibility() {
        int[] selection = resTypeslist.getSelectionIndices();
        if (null != selection && selection.length > 0) {
            remFromListBtn.setEnabled(true);
        } else {
            remFromListBtn.setEnabled(false);
        }
    }

    @Override
    public boolean canFlipToNextPage() {
        System.out.println("canFlipToNextPage()");
        if (!anyResourceExist) {
            return false;
        }
        return isSelectionDone();
    }

    public boolean isSelectionDone() {
        if (null == resName || resName.trim().length() < 1 || null == resURI
                || resURI.trim().length() < 1 || null == resTypes
                || resTypes.size() < 1) {
            return false;
        }
        return true;
    }

    @Override
    public IWizardPage getNextPage() {
        // Checking whether the uri is used by any other resource.
        if (Activator.getDefault().getResourceManager().isResourceExist(resURI)) {
            MessageDialog
                    .openError(getShell(), "Resource URI in use",
                            "Entered resource URI is in use. Please try a different one.");
            // TODO: Instead of MessageDialog, errors may be shown on wizard
            // itself.
            return null;
        }

        if (!Utility.isUriValid(resURI)) {
            MessageDialog.openError(Display.getDefault().getActiveShell(),
                    "Invalid Resource URI.", Constants.INVALID_URI_MESSAGE);
            return null;
        }

        if (anyResourceExist) {
            return ((CreateResourceWizard) getWizard())
                    .getAddResourcesToCollectionPage();
        }
        return null;
    }

    public String getResName() {
        return resName;
    }

    public String getResURI() {
        return resURI;
    }

    public Set<String> getResTypes() {
        return resTypes;
    }

    /*
     * public boolean isStart() { return start; }
     */

    public boolean isObservable() {
        return observable;
    }

    public boolean isAnyResourceExist() {
        return anyResourceExist;
    }
}
