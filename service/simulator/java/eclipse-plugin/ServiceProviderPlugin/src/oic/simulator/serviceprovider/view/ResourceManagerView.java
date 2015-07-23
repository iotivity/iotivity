package oic.simulator.serviceprovider.view;

import java.util.Iterator;
import java.util.List;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.listener.IResourceListChangedListener;
import oic.simulator.serviceprovider.manager.ResourceManager;
import oic.simulator.serviceprovider.utils.Convertion;
import oic.simulator.serviceprovider.view.dialogs.CreateResourceWizard;
import oic.simulator.serviceprovider.view.dialogs.DeleteCategory;
import oic.simulator.serviceprovider.view.dialogs.DeleteResourceWizard;
import oic.simulator.serviceprovider.view.dialogs.ResourceWizardDialog;

import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

public class ResourceManagerView extends ViewPart {

    public static final String           VIEW_ID = "oic.simulator.serviceprovider.view.resourcemanager";

    private Button                       createButton;
    private Button                       deleteButton;

    private Tree                         resourceTreeHead;

    private IResourceListChangedListener resourceListChangedListener;

    public ResourceManagerView() {
        resourceListChangedListener = new IResourceListChangedListener() {

            @Override
            public void onResourceCreation(String resourceType,
                    List<String> resourceURI) {
                if (null != resourceTreeHead
                        && !(resourceTreeHead.isDisposed())
                        && null != resourceType && null != resourceURI) {
                    TreeItem resourceHead = null;
                    TreeItem[] items = resourceTreeHead.getItems();
                    if (null != items) {
                        for (TreeItem item : items) {
                            if (item.getText().equals(resourceType)) {
                                resourceHead = item;
                                break;
                            }
                        }
                        if (null == resourceHead) {
                            // Adding a new tree header
                            resourceHead = new TreeItem(resourceTreeHead,
                                    SWT.NULL);
                            resourceHead.setText(resourceType);
                        }
                        // Add resources under the resourceHead
                        Iterator<String> uriItr = resourceURI.iterator();
                        while (uriItr.hasNext()) {
                            TreeItem newItem = new TreeItem(resourceHead,
                                    SWT.NULL);
                            newItem.setText(Convertion.uriToDisplayName(uriItr
                                    .next()));
                        }
                    }
                }
            }

            @Override
            public void onResourceDeletion(String resourceType,
                    String resourceURI) {
                if (null != resourceTreeHead
                        && !(resourceTreeHead.isDisposed())) {
                    TreeItem targetParent = null;
                    TreeItem[] headItems = resourceTreeHead.getItems();
                    if (null != headItems) {
                        for (TreeItem item : headItems) {
                            if (item.getText().equals(resourceType)) {
                                targetParent = item;
                                break;
                            }
                        }
                        if (null != targetParent) {
                            TreeItem[] items = targetParent.getItems();
                            for (TreeItem item : items) {
                                if (item.getText().equals(
                                        Convertion
                                                .uriToDisplayName(resourceURI))) {
                                    item.dispose();
                                    if (targetParent.getItemCount() < 1) {
                                        targetParent.dispose();
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            @Override
            public void onResourceDeletionByType(String resourceType) {
                if (null != resourceTreeHead
                        && !(resourceTreeHead.isDisposed())) {
                    TreeItem items[] = resourceTreeHead.getItems();
                    for (TreeItem item : items) {
                        if (item.getText().equals(resourceType)) {
                            item.dispose();
                            break;
                        }
                    }
                }
            }

            public void onAllResourceDeletion() {
                if (null != resourceTreeHead
                        && !(resourceTreeHead.isDisposed())) {
                    TreeItem[] items = resourceTreeHead.getItems();
                    if (null != items) {
                        for (TreeItem item : items) {
                            item.dispose();
                        }
                    }
                }
            }
        };
    }

    @Override
    public void createPartControl(Composite parent) {
        Composite compContent = new Composite(parent, SWT.NONE);
        GridLayout baseLayout = new GridLayout(1, false);
        compContent.setLayout(baseLayout);

        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        compContent.setLayoutData(gd);

        Composite buttonComp = new Composite(compContent, SWT.NONE);
        buttonComp.setLayout(new GridLayout(2, false));

        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        buttonComp.setLayoutData(gd);

        createButton = new Button(buttonComp, SWT.PUSH);
        createButton.setText("Create");
        createButton.setToolTipText("Create Simulator Resource(s)");

        gd = new GridData();
        gd.widthHint = 90;
        createButton.setLayoutData(gd);

        deleteButton = new Button(buttonComp, SWT.PUSH);
        deleteButton.setText("Delete");
        deleteButton.setToolTipText("Delete Simulator Resource(s)");

        gd = new GridData();
        gd.widthHint = 90;
        deleteButton.setLayoutData(gd);

        Group resourceGroup = new Group(compContent, SWT.NONE);
        resourceGroup.setText("Created Resources");

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        resourceGroup.setBackground(color);

        resourceGroup.setLayout(new GridLayout(1, false));
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        resourceGroup.setLayoutData(gd);

        resourceTreeHead = new Tree(resourceGroup, SWT.SINGLE);

        gd = new GridData(SWT.FILL, SWT.FILL, true, true);

        resourceTreeHead.setLayoutData(gd);

        addUIListeners();

        addManagerListeners();
    }

    private void addUIListeners() {

        createButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Runnable() {

                    @Override
                    public void run() {
                        CreateResourceWizard createWizard = new CreateResourceWizard();
                        ResourceWizardDialog wizardDialog = new ResourceWizardDialog(
                                PlatformUI.getWorkbench().getDisplay()
                                        .getActiveShell(), createWizard);
                        int open = wizardDialog.open();
                        if (open == WizardDialog.OK) {
                            String configFilePath;
                            int count;
                            configFilePath = createWizard.getConfigFilePath();
                            count = createWizard.getResourceCount();

                            // Create the resource
                            if (null != configFilePath
                                    && configFilePath.length() > 0) {
                                if (count <= 1) {
                                    // Single resource creation
                                    // TODO: Temporary code to show the resource
                                    // in UI
                                    Activator.getManager().createResource(
                                            configFilePath);
                                } else {
                                    // Multi resource creation
                                    // Activator.getDefault().getManager().createResource(configFilePath,
                                    // count);
                                }
                            }
                        }
                    }
                });
            }
        });

        deleteButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Runnable() {

                    @Override
                    public void run() {
                        DeleteResourceWizard deleteWizard = new DeleteResourceWizard();
                        ResourceWizardDialog wizardDialog = new ResourceWizardDialog(
                                PlatformUI.getWorkbench().getDisplay()
                                        .getActiveShell(), deleteWizard);
                        int open = wizardDialog.open();
                        if (open == WizardDialog.OK) {
                            if (deleteWizard.getDeleteCategory() == DeleteCategory.BY_URI) {
                                String uri = deleteWizard.getDeleteCandidate();
                                boolean completeURI = Convertion
                                        .isUriComplete(uri);
                                if (!completeURI) {
                                    uri = Convertion.displayNameToUri(uri);
                                }
                                Activator.getManager().deleteResourceByURI(uri);
                            } else if (deleteWizard.getDeleteCategory() == DeleteCategory.BY_TYPE) {
                                Activator.getManager().deleteResourceByType(
                                        deleteWizard.getDeleteCandidate());
                            } else if (deleteWizard.getDeleteCategory() == DeleteCategory.ALL) {
                                Activator.getManager().deleteAllResources();
                            }
                        }
                    }

                });
            }
        });

        resourceTreeHead.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                TreeItem[] selectedItems = resourceTreeHead.getSelection();
                // Handling only single item selection events
                if (1 == selectedItems.length) {
                    String selectedItem = selectedItems[0].getText();
                    // Check whether the selected item is a resource category or
                    // a specific resource
                    boolean category = isSelectedTreeItemACategory(selectedItem);
                    if (!category) {
                        // TODO: Show the properties in the properties view
                    }

                }
            }
        });
    }

    private boolean isSelectedTreeItemACategory(String selectedItem) {
        boolean category = false;
        TreeItem[] items = resourceTreeHead.getItems();
        if (null != items) {
            for (TreeItem item : items) {
                if (item.getText().equals(selectedItem)) {
                    category = true;
                    break;
                }
            }
        }
        return category;
    }

    public void addManagerListeners() {
        ResourceManager manager = Activator.getManager();
        manager.setResourceListChangedListener(resourceListChangedListener);
    }

    @Override
    public void setFocus() {
        // TODO Auto-generated method stub

    }

}
