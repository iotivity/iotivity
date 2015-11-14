package oic.simulator.serviceprovider.view.dialogs;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

public class DevicePage extends WizardPage {

    private CheckboxTreeViewer treeViewer;

    private Text               deviceNameTxt;

    private String             deviceName;

    Set<Resource>              selectedResourceList;

    protected DevicePage() {
        super("Devices");
        selectedResourceList = new HashSet<Resource>();
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.CREATE_DEVICE_PAGE_TITLE);
        setMessage(Constants.CREATE_DEVICE_PAGE_MESSAGE);

        Composite container = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout(2, false);
        container.setLayout(gridLayout);
        GridData gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.heightHint = 300;
        container.setLayoutData(gd);

        Label deviceNameLbl = new Label(container, SWT.NULL);
        deviceNameLbl.setText("Device Name");

        deviceNameTxt = new Text(container, SWT.BORDER);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        deviceNameTxt.setLayoutData(gd);

        Group resourceGroup = new Group(container, SWT.NONE);

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);

        resourceGroup.setLayout(new GridLayout());
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.heightHint = 300;
        gd.horizontalSpan = 2;
        resourceGroup.setLayoutData(gd);

        treeViewer = new CheckboxTreeViewer(resourceGroup);
        treeViewer.getTree().setBackground(color);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessVerticalSpace = true;
        gd.verticalAlignment = SWT.FILL;
        // gd.heightHint = 300;
        treeViewer.getTree().setLayoutData(gd);
        treeViewer.setContentProvider(new TreeContentProvider());
        treeViewer.setLabelProvider(new TreeLabelProvider());
        treeViewer.setInput(new Object());
        treeViewer.addCheckStateListener(new ICheckStateListener() {

            @Override
            public void checkStateChanged(CheckStateChangedEvent e) {
                Resource res = (Resource) e.getElement();
                if (null != res) {
                    if (e.getChecked()) {
                        selectedResourceList.add(res);
                        System.out.println("Checked" + res.getResourceName());
                    } else {
                        selectedResourceList.remove(res);
                        System.out.println("Unchecked:" + res.getResourceName());
                    }
                }
            }
        });

        setUiListeners();

        setControl(container);
    }

    private void setUiListeners() {
        deviceNameTxt.addModifyListener(new ModifyListener() {
            @Override
            public void modifyText(ModifyEvent e) {
                deviceName = deviceNameTxt.getText();
                setPageComplete(isSelectionDone());
            }
        });
    }

    private boolean isSelectionDone() {
        if (null != deviceName) {
            deviceName = deviceName.trim();
        }
        if (deviceName.isEmpty()) {
            return false;
        }
        return true;
    }

    @Override
    public IWizardPage getNextPage() {
        return null;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public Set<Resource> getSelectedResourceList() {
        return selectedResourceList;
    }

}

class TreeContentProvider implements ITreeContentProvider {

    @Override
    public void dispose() {
    }

    @Override
    public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
    }

    @Override
    public Object[] getChildren(Object parent) {
        return null;
    }

    @Override
    public Object[] getElements(Object parent) {
        List<Resource> resList;
        resList = Activator.getDefault().getResourceManager().getResourceList();
        if (null == resList) {
            resList = new ArrayList<Resource>();
        }
        return resList.toArray();
    }

    @Override
    public Object getParent(Object child) {
        return null;
    }

    @Override
    public boolean hasChildren(Object parent) {
        return false;
    }
}

class TreeLabelProvider extends LabelProvider {
    @Override
    public String getText(Object element) {
        Resource res = (Resource) element;
        return res.getResourceName();
    }

    @Override
    public Image getImage(Object element) {
        if (element instanceof SingleResource) {
            return Activator.getDefault().getImageRegistry()
                    .get(Constants.SINGLE_RESOURCE);
        } else {
            return Activator.getDefault().getImageRegistry()
                    .get(Constants.COLLECTION_RESOURCE);
        }
    }
}