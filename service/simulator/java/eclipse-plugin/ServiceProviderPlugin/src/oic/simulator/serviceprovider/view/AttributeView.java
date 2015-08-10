package oic.simulator.serviceprovider.view;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.listener.IAutomationUIListener;
import oic.simulator.serviceprovider.listener.IResourceModelChangedUIListener;
import oic.simulator.serviceprovider.listener.IResourceSelectionChangedUIListener;
import oic.simulator.serviceprovider.manager.ResourceManager;
import oic.simulator.serviceprovider.resource.AutomationSettingHelper;
import oic.simulator.serviceprovider.resource.ModelChangeNotificationType;
import oic.simulator.serviceprovider.resource.ResourceAttribute;
import oic.simulator.serviceprovider.resource.SimulatorResource;
import oic.simulator.serviceprovider.utils.Constants;
import oic.simulator.serviceprovider.utils.Utility;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CellLabelProvider;
import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StyledCellLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerCell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.TraverseEvent;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.events.VerifyListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.ui.part.ViewPart;
import org.oic.simulator.AutomationType;

public class AttributeView extends ViewPart {

    public static final String                  VIEW_ID            = "oic.simulator.serviceprovider.view.attribute";

    private TableViewer                         attTblViewer;
    private TableViewer                         automtnSettingsTblViewer;

    private IResourceSelectionChangedUIListener resourceSelectionChangedListener;
    private IResourceModelChangedUIListener     resourceModelChangedUIListener;
    private IAutomationUIListener               automationUIListener;

    private Label                               newValueLbl;
    private CCombo                              valueCmb;
    private Button                              submitButton;
    private Button                              clearButton;

    private ResourceAttribute                   attributeInSelection;
    private AttributeAutomationSettingEditor    automationSettingEditor;
    private List<AutomationSettingHelper>       localSettingList;

    private final String[]                      attTblHeaders      = { "Name",
            "Value", "Automation Status"                          };
    private final String[]                      settingTblHeaders  = {
            "Setting", "Value"                                    };
    private final Integer[]                     attTblColWidth     = { 150,
            190, 150                                              };
    private final Integer[]                     settingTblColWidth = { 140, 85 };

    private ResourceManager                     resourceManager;

    public AttributeView() {

        resourceManager = Activator.getDefault().getResourceManager();

        resourceSelectionChangedListener = new IResourceSelectionChangedUIListener() {

            @Override
            public void onResourceSelectionChange() {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null != attTblViewer) {
                            updateViewer(checkSelection());
                        }
                    }
                });
            }
        };

        resourceModelChangedUIListener = new IResourceModelChangedUIListener() {

            @Override
            public void onResourceModelChange(
                    final ModelChangeNotificationType notificationType,
                    final String resourceURI) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        // Handle the notification only if it is for the current
                        // resource in selection
                        SimulatorResource resource = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resource) {
                            return;
                        }
                        if (!resourceURI.equals(resource.getResourceURI())) {
                            return;
                        }
                        // Refresh the table viewers which will display
                        // the updated values
                        if (null != attTblViewer) {
                            attTblViewer.refresh();
                        }
                        // If this call has a new value for the current
                        // attribute
                        // in selection, then update it in the combo box
                        if (notificationType != ModelChangeNotificationType.ATTRIBUTE_VALUE_CHANGED) {
                            return;
                        }
                        if (null != valueCmb && !valueCmb.isDisposed()
                                && null != attributeInSelection) {
                            Map<String, ResourceAttribute> attributeMap = resource
                                    .getResourceAttributesMap();
                            if (null != attributeMap) {
                                ResourceAttribute attribute = attributeMap
                                        .get(attributeInSelection
                                                .getAttributeName());
                                if (null != attribute) {
                                    Object valueObj = attribute
                                            .getAttributeValue();
                                    if (null != valueObj) {
                                        String valueStr = String
                                                .valueOf(valueObj);
                                        int index = valueCmb.indexOf(valueStr);
                                        if (index != -1) {
                                            valueCmb.select(index);
                                        } else {
                                            valueCmb.add(valueStr);
                                            valueCmb.select(valueCmb
                                                    .indexOf(valueStr));
                                        }
                                    }
                                }
                            }
                        }
                    }
                });
            }
        };

        automationUIListener = new IAutomationUIListener() {

            @Override
            public void onResourceAutomationStart(final String resourceURI) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resourceURI) {
                            return;
                        }
                        SimulatorResource resource = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resource) {
                            return;
                        }
                        String uri = resource.getResourceURI();
                        // Checking whether attributes view is currently
                        // displaying the attributes of the
                        // resource whose automation has just started
                        if (null != uri && uri.equals(resourceURI)) {
                            Table tbl;
                            tbl = attTblViewer.getTable();
                            if (!tbl.isDisposed()) {
                                attTblViewer.refresh();
                            }

                            // If any attribute is in selection, then update
                            // the automation setting to enable.
                            if (null != attributeInSelection) {
                                tbl = automtnSettingsTblViewer.getTable();
                                if (!tbl.isDisposed()) {
                                    // Change the local automation settings
                                    AutomationSettingHelper
                                            .updateAutomationStatus(
                                                    localSettingList,
                                                    Constants.ENABLE);
                                    automtnSettingsTblViewer.refresh();
                                }
                            }

                            // Disable the manual change UI controls
                            setVisibilityForManualValueChange(false);
                        }
                    }
                });
            }

            @Override
            public void onAutomationComplete(final String resourceURI,
                    final String attName) {
                // This method notifies the completion of attribute level
                // automation.
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resourceURI) {
                            return;
                        }
                        // Check if the given resourceURI is the uri of the
                        // resource whose attributes are currently being
                        // displayed by this view.
                        SimulatorResource resource = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resource) {
                            return;
                        }
                        String uri = resource.getResourceURI();
                        if (null != uri && uri.equals(resourceURI)) {
                            Table tbl;
                            tbl = attTblViewer.getTable();
                            if (!tbl.isDisposed()) {
                                attTblViewer.refresh();
                            }

                            if (null != attributeInSelection) {
                                if (null == attName
                                        || attributeInSelection
                                                .getAttributeName().equals(
                                                        attName)) {
                                    tbl = automtnSettingsTblViewer.getTable();
                                    if (!tbl.isDisposed()) {
                                        // Change the local automation settings
                                        AutomationSettingHelper
                                                .updateAutomationStatus(
                                                        localSettingList,
                                                        Constants.DISABLE);
                                        automtnSettingsTblViewer.refresh();
                                    }
                                    setVisibilityForManualValueChange(true);
                                }
                            }
                        }
                    }
                });
            }
        };
    }

    @Override
    public void createPartControl(Composite parent) {
        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);

        parent.setLayout(new GridLayout(2, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        parent.setLayoutData(gd);

        Group attGroup = new Group(parent, SWT.NONE);
        attGroup.setLayout(new GridLayout(1, false));
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = 2;
        gd.minimumHeight = 175;
        attGroup.setLayoutData(gd);
        attGroup.setText("Attributes");
        attGroup.setBackground(color);

        attTblViewer = new TableViewer(attGroup, SWT.MULTI | SWT.H_SCROLL
                | SWT.V_SCROLL | SWT.FULL_SELECTION | SWT.BORDER);

        createAttributeColumns(attTblViewer);

        // make lines and header visible
        Table table = attTblViewer.getTable();
        table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        table.setHeaderVisible(true);
        table.setLinesVisible(true);

        attTblViewer.setContentProvider(new AttributeContentProvider());

        Group automationSettingsTableGrp = new Group(parent, SWT.NONE);
        automationSettingsTableGrp.setLayout(new GridLayout(1, false));
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        automationSettingsTableGrp.setLayoutData(gd);
        automationSettingsTableGrp.setText("Automation Settings");
        automationSettingsTableGrp.setBackground(color);

        automtnSettingsTblViewer = new TableViewer(automationSettingsTableGrp,
                SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.FULL_SELECTION
                        | SWT.BORDER);

        createSettingColumns(automtnSettingsTblViewer);

        // make lines visible
        table = automtnSettingsTblViewer.getTable();
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        table.setLayoutData(gd);
        table.setHeaderVisible(true);
        table.setLinesVisible(true);

        automtnSettingsTblViewer
                .setContentProvider(new AttributeContentProvider());

        Composite rightViewComp = new Composite(parent, SWT.NONE);
        rightViewComp.setLayout(new GridLayout(1, false));

        Group valueUpdateGroup = new Group(rightViewComp, SWT.NONE);
        valueUpdateGroup.setLayout(new GridLayout(2, false));
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 2;
        valueUpdateGroup.setLayoutData(gd);
        valueUpdateGroup.setText("Change Attribute Value");
        valueUpdateGroup.setBackground(color);

        newValueLbl = new Label(valueUpdateGroup, SWT.NONE);
        newValueLbl.setText("Select New Value");

        valueCmb = new CCombo(valueUpdateGroup, SWT.BORDER);
        gd = new GridData();
        gd.widthHint = 100;
        valueCmb.setLayoutData(gd);

        submitButton = new Button(rightViewComp, SWT.PUSH);
        gd = new GridData();
        gd.widthHint = 150;
        gd.verticalIndent = 10;
        gd.horizontalSpan = 2;
        gd.horizontalAlignment = SWT.CENTER;
        submitButton.setLayoutData(gd);
        submitButton.setText("Apply Changes");

        clearButton = new Button(rightViewComp, SWT.PUSH);
        gd = new GridData();
        gd.widthHint = 150;
        gd.verticalIndent = 10;
        gd.horizontalSpan = 2;
        gd.horizontalAlignment = SWT.CENTER;
        clearButton.setLayoutData(gd);
        clearButton.setText("Reset Changes");

        addUIListeners();

        addManagerListeners();

        // Check whether there is any resource selected already
        List<ResourceAttribute> propertyList = checkSelection();
        if (null != propertyList) {
            updateViewer(propertyList);
        }

        setSettingSubmitVisibility(false);
    }

    public void createAttributeColumns(TableViewer tableViewer) {
        TableViewerColumn attName = new TableViewerColumn(tableViewer, SWT.NONE);
        attName.getColumn().setWidth(attTblColWidth[0]);
        attName.getColumn().setText(attTblHeaders[0]);
        attName.setLabelProvider(new ColumnLabelProvider() {
            @Override
            public String getText(Object element) {
                if (element instanceof ResourceAttribute) {
                    ResourceAttribute attribute = (ResourceAttribute) element;
                    if (null != attribute) {
                        return attribute.getAttributeName();
                    }
                }
                return "";
            }
        });

        TableViewerColumn attValue = new TableViewerColumn(tableViewer,
                SWT.NONE);
        attValue.getColumn().setWidth(attTblColWidth[1]);
        attValue.getColumn().setText(attTblHeaders[1]);
        attValue.setLabelProvider(new ColumnLabelProvider() {
            @Override
            public String getText(Object element) {
                if (element instanceof ResourceAttribute) {
                    ResourceAttribute attribute = (ResourceAttribute) element;
                    if (null != attribute) {
                        Object val = attribute.getAttributeValue();
                        if (null != val) {
                            return String.valueOf(val);
                        }
                    }
                }
                return "";
            }
        });

        TableViewerColumn automationStatus = new TableViewerColumn(tableViewer,
                SWT.NONE);
        automationStatus.getColumn().setWidth(attTblColWidth[2]);
        automationStatus.getColumn().setText(attTblHeaders[2]);
        automationStatus.setLabelProvider(new ColumnLabelProvider() {
            @Override
            public String getText(Object element) {
                if (element instanceof ResourceAttribute) {
                    ResourceAttribute attribute = (ResourceAttribute) element;
                    if (null != attribute) {
                        boolean progress = attribute.isAutomationInProgress();
                        return Utility.getAutomationStatus(progress);
                    }
                }
                return "";
            }
        });
    }

    public void createSettingColumns(TableViewer tableViewer) {
        TableViewerColumn key = new TableViewerColumn(tableViewer, SWT.NONE);
        key.getColumn().setWidth(settingTblColWidth[0]);
        key.getColumn().setText(settingTblHeaders[0]);
        key.setLabelProvider(new StyledCellLabelProvider() {
            @Override
            public void update(ViewerCell cell) {
                Object element = cell.getElement();
                if (element instanceof AutomationSettingHelper) {
                    AutomationSettingHelper setting = (AutomationSettingHelper) element;
                    cell.setText(setting.getSettingID());
                }
            }
        });

        TableViewerColumn value = new TableViewerColumn(tableViewer, SWT.NONE);
        value.getColumn().setWidth(settingTblColWidth[1]);
        value.getColumn().setText(settingTblHeaders[1]);
        value.setLabelProvider(new CellLabelProvider() {
            @Override
            public void update(ViewerCell cell) {
                Object element = cell.getElement();
                if (element instanceof AutomationSettingHelper) {
                    AutomationSettingHelper setting = (AutomationSettingHelper) element;
                    if (null != setting) {
                        cell.setText(String.valueOf(setting.getSettingValue()));
                    }
                }
            }
        });
        automationSettingEditor = new AttributeAutomationSettingEditor(
                tableViewer);
        value.setEditingSupport(automationSettingEditor);
    }

    private void addUIListeners() {
        attTblViewer
                .addSelectionChangedListener(new ISelectionChangedListener() {

                    @Override
                    public void selectionChanged(SelectionChangedEvent e) {
                        IStructuredSelection selection = (IStructuredSelection) attTblViewer
                                .getSelection();
                        Object firstElement = selection.getFirstElement();
                        if (firstElement instanceof ResourceAttribute) {
                            ResourceAttribute attribute = (ResourceAttribute) firstElement;

                            // Store the attribute selection
                            attributeInSelection = attribute;

                            // Change the visibility of the manual change
                            // controls according to the automation status
                            if (attribute.isAutomationInProgress()) {
                                setVisibilityForManualValueChange(false);
                            } else {
                                setVisibilityForManualValueChange(true);
                            }

                            // Hide the visibility of submit and clear buttons
                            setSettingSubmitVisibility(false);

                            if (null != attribute) {
                                if (null != automtnSettingsTblViewer) {
                                    localSettingList = AutomationSettingHelper
                                            .getAutomationSettings(attribute);
                                    if (null != localSettingList) {
                                        automtnSettingsTblViewer.getTable()
                                                .setLinesVisible(true);
                                        automtnSettingsTblViewer
                                                .setInput(localSettingList
                                                        .toArray());
                                    }
                                }
                                // Populate the attribute value combo
                                if (null != valueCmb && !valueCmb.isDisposed()) {
                                    // Clear the existing combo list contents
                                    valueCmb.removeAll();
                                    Set<Object> valueList = attribute
                                            .getValues();
                                    Object value;
                                    if (null != valueList) {
                                        Iterator<Object> valueItr = valueList
                                                .iterator();
                                        while (valueItr.hasNext()) {
                                            value = valueItr.next();
                                            if (null != value) {
                                                // Casting the value to String
                                                // for showing in combo
                                                valueCmb.add(String
                                                        .valueOf(value));
                                            }
                                        }
                                        // Setting the selection to the current
                                        // value of the attribute
                                        String attValue = String
                                                .valueOf(attribute
                                                        .getAttributeValue());
                                        int index = valueCmb.indexOf(attValue);
                                        if (index != -1) {
                                            valueCmb.select(index);
                                        }
                                    }
                                }
                            }
                        }
                    }
                });

        valueCmb.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (null != attributeInSelection) {
                    // attributeInSelection.setAttributeValue(valueCmb.getText());
                    if (!valueCmb.getText().equals(
                            String.valueOf(attributeInSelection
                                    .getAttributeValue()))) {
                        // Enable the submit and clear button controls
                        setSettingSubmitVisibility(true);
                    } else {
                        setSettingSubmitVisibility(false);
                    }
                }
            }
        });

        submitButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {

                if (null != attributeInSelection && null != localSettingList) {
                    if (attributeInSelection.isAutomationInProgress()
                            || valueCmb.getText().equals(
                                    String.valueOf(attributeInSelection
                                            .getAttributeValue()))) {
                        // Change in automation settings
                        Iterator<AutomationSettingHelper> settingItr = localSettingList
                                .iterator();

                        // These variables will hold the new automation
                        // settings.
                        boolean autoEnable = false;
                        AutomationType autoType = AutomationType.NORMAL;
                        int autoUpdateInterval = Constants.DEFAULT_AUTOMATION_INTERVAL;

                        // These variables will hold the old automation settings
                        // and are used to roll-back the settings
                        // if automation fails.
                        boolean autoEnablePrev = false;
                        AutomationType autoTypePrev = AutomationType.NORMAL;
                        int autoUpdateIntervalPrev = Constants.DEFAULT_AUTOMATION_INTERVAL;

                        String id;
                        String value;
                        AutomationSettingHelper setting;
                        while (settingItr.hasNext()) {
                            setting = settingItr.next();
                            id = setting.getSettingID();
                            value = setting.getSettingValue();
                            if (id.equals(Constants.AUTOMATION)) {
                                autoEnable = Utility
                                        .getAutomationBoolean(value);
                                autoEnablePrev = attributeInSelection
                                        .isAutomationInProgress();

                                attributeInSelection
                                        .setAutomationInProgress(autoEnable);
                                // Automation is disabled and hence the manual
                                // controls should be enabled
                                if (!autoEnable) {
                                    setVisibilityForManualValueChange(true);
                                }
                            } else if (id.equals(Constants.AUTOMATION_TYPE)) {
                                autoType = AutomationType.valueOf(value);
                                autoTypePrev = attributeInSelection
                                        .getAutomationType();

                                attributeInSelection
                                        .setAutomationType(autoType);
                            } else if (id
                                    .equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                                autoUpdateInterval = Utility
                                        .getUpdateIntervalFromString(value);
                                autoUpdateIntervalPrev = attributeInSelection
                                        .getAutomationUpdateInterval();

                                attributeInSelection
                                        .setAutomationUpdateInterval(autoUpdateInterval);
                            }
                        }

                        // Updating the automation status in the resource
                        SimulatorResource resource = resourceManager
                                .getCurrentResourceInSelection();
                        resource.setAttributeAutomationInProgress(autoEnable);

                        if (autoEnable) {
                            // Send automation enable request
                            int autoId = resourceManager.startAutomation(
                                    resource, attributeInSelection, autoType,
                                    autoUpdateInterval);
                            if (-1 == autoId) {
                                // Automation failed
                                // Roll-back the attribute model and automation
                                // settings
                                settingItr = localSettingList.iterator();
                                while (settingItr.hasNext()) {
                                    setting = settingItr.next();
                                    id = setting.getSettingID();
                                    value = setting.getSettingValue();
                                    if (id.equals(Constants.AUTOMATION)) {
                                        setting.setSettingValue(Utility
                                                .getAutomationString(autoEnablePrev));
                                    } else if (id
                                            .equals(Constants.AUTOMATION_TYPE)) {
                                        setting.setSettingValue(autoTypePrev
                                                .toString());
                                    } else if (id
                                            .equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                                        setting.setSettingValue(String
                                                .valueOf(autoUpdateIntervalPrev));
                                    }
                                }

                                attributeInSelection
                                        .setAutomationInProgress(autoEnablePrev);
                                attributeInSelection
                                        .setAutomationType(autoTypePrev);
                                attributeInSelection
                                        .setAutomationUpdateInterval(autoUpdateIntervalPrev);

                                automtnSettingsTblViewer.refresh();

                                // Automation is disabled and hence the manual
                                // controls should be enabled
                                setVisibilityForManualValueChange(true);
                            }
                        } else {
                            // Send automation disable request
                            int autoId = attributeInSelection.getAutomationId();
                            resourceManager.stopAutomation(resource, autoId);
                        }
                    } else {
                        // Change in attribute value manually
                        String input = valueCmb.getText();
                        if (null != input && input.length() > 0) {
                            attributeInSelection.setAttributeValue(input);

                            SimulatorResource resource = resourceManager
                                    .getCurrentResourceInSelection();
                            resourceManager.attributeValueUpdated(resource,
                                    attributeInSelection.getAttributeName(),
                                    input);
                        }
                    }
                }

                attTblViewer.refresh();

                setSettingSubmitVisibility(false);
            }
        });

        clearButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (null != attributeInSelection) {
                    // Update the settings table
                    List<AutomationSettingHelper> settingList = AutomationSettingHelper
                            .getAutomationSettings(attributeInSelection);
                    if (null != settingList) {
                        // clear the existing table contents
                        Table tbl = automtnSettingsTblViewer.getTable();
                        if (!tbl.isDisposed()) {
                            tbl.removeAll();
                            localSettingList = settingList;
                            automtnSettingsTblViewer.setInput(localSettingList
                                    .toArray());
                        }
                    }
                    // Update the allowed values combo
                    if (!valueCmb.isDisposed()) {
                        valueCmb.setText(String.valueOf(attributeInSelection
                                .getAttributeValue()));
                        setVisibilityForManualValueChange(!attributeInSelection
                                .isAutomationInProgress());
                    }
                }
                setSettingSubmitVisibility(false);
            }
        });
    }

    private void addManagerListeners() {
        resourceManager
                .addResourceSelectionChangedUIListener(resourceSelectionChangedListener);
        resourceManager
                .addResourceModelChangedUIListener(resourceModelChangedUIListener);
        resourceManager.addAutomationUIListener(automationUIListener);
    }

    private void setSettingSubmitVisibility(boolean visible) {
        if (!submitButton.isDisposed())
            submitButton.setEnabled(visible);
        if (!clearButton.isDisposed())
            clearButton.setEnabled(visible);
    }

    private List<ResourceAttribute> checkSelection() {
        SimulatorResource resourceInSelection = resourceManager
                .getCurrentResourceInSelection();
        if (null != resourceInSelection) {
            List<ResourceAttribute> attList = resourceManager
                    .getAttributes(resourceInSelection);
            return attList;
        } else {
            return null;
        }
    }

    private void updateViewer(List<ResourceAttribute> attList) {
        Table tbl;
        if (null != attList) {
            tbl = attTblViewer.getTable();
            if (null != tbl && !tbl.isDisposed()) {
                tbl.setLinesVisible(true);
                attTblViewer.setInput(attList.toArray());
            }
        } else {
            // Clear the attributes table viewer
            if (null != attTblViewer) {
                tbl = attTblViewer.getTable();
                if (null != tbl && !tbl.isDisposed()) {
                    // tbl.deselectAll();
                    tbl.removeAll();
                    tbl.setLinesVisible(false);
                }
            }
        }

        attributeInSelection = null;

        // Clear the Settings table viewer
        if (null != automtnSettingsTblViewer) {
            tbl = automtnSettingsTblViewer.getTable();
            if (null != tbl && !tbl.isDisposed()) {
                // tbl.deselectAll();
                tbl.removeAll();
                tbl.setLinesVisible(false);
            }
        }
        // Clear the value list combo
        if (!valueCmb.isDisposed()) {
            valueCmb.removeAll();
            valueCmb.setEnabled(false);
        }

        // Hide the visibility of submit and clear buttons
        setSettingSubmitVisibility(false);
    }

    class AttributeContentProvider implements IStructuredContentProvider {

        @Override
        public void dispose() {
        }

        @Override
        public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
        }

        @Override
        public Object[] getElements(Object element) {
            return (Object[]) element;
        }

    }

    class AttributeAutomationSettingEditor extends EditingSupport {

        private final TableViewer       viewer;

        private AutomationSettingHelper currentSettingInSelection;

        public AttributeAutomationSettingEditor(TableViewer viewer) {
            super(viewer);
            this.viewer = viewer;
        }

        @Override
        protected boolean canEdit(Object element) {
            return true;
        }

        @Override
        protected CellEditor getCellEditor(Object element) {
            ComboBoxCellEditor combo = null;
            if (null != element) {
                List<String> valueList;
                boolean readOnly = true;
                AutomationSettingHelper setting = (AutomationSettingHelper) element;

                currentSettingInSelection = setting;

                String settingID = setting.getSettingID();
                if (settingID.equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                    readOnly = false;
                }
                valueList = setting.getAllowedValues();
                if (null == valueList) {
                    valueList = new ArrayList<String>();
                }
                // Convert list to String[] for passing it to ComboBoxCellEditor
                String[] values;
                values = valueList.toArray(new String[1]);
                if (readOnly) {
                    combo = new ComboBoxCellEditor(viewer.getTable(), values,
                            SWT.READ_ONLY);
                } else {
                    combo = new ComboBoxCellEditor(viewer.getTable(), values);
                }

                // If resource level automation is enabled, then all setting
                // should be disabled
                SimulatorResource resourceInSelection;
                resourceInSelection = resourceManager
                        .getCurrentResourceInSelection();
                if (null != resourceInSelection
                        && resourceInSelection.isResourceAutomationInProgress()) {
                    CCombo c = (CCombo) combo.getControl();
                    c.setEnabled(false);
                } else {
                    // Enable the automation type and update interval combo
                    // boxes only if the current value of automation is 'enable'
                    if (settingID.equals(Constants.AUTOMATION_TYPE)
                            || settingID
                                    .equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                        if (!showSettings()) {
                            // Disable the combo box
                            CCombo c = (CCombo) combo.getControl();
                            c.setEnabled(false);
                        }
                    }
                }
                // Enabling editing support in update interval combo box
                if (settingID.equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                    final CCombo c = (CCombo) combo.getControl();
                    c.addVerifyListener(new VerifyListener() {
                        @Override
                        public void verifyText(VerifyEvent e) {
                            String text = c.getText();
                            String newText = text.substring(0, e.start)
                                    + e.text + text.substring(e.end);
                            try {
                                if (newText.length() != 0) {
                                    Integer.parseInt(newText);
                                }
                            } catch (NumberFormatException ex) {
                                e.doit = false;
                            }
                        }
                    });
                    c.addTraverseListener(new TraverseListener() {
                        @Override
                        public void keyTraversed(TraverseEvent e) {
                            if (e.detail == SWT.TRAVERSE_RETURN) {
                                e.doit = false;
                                e.detail = SWT.TRAVERSE_NONE;
                                String newText = c.getText();
                                try {
                                    Integer.parseInt(newText);
                                    if (null != currentSettingInSelection
                                            && currentSettingInSelection
                                                    .getSettingID()
                                                    .equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                                        currentSettingInSelection
                                                .addAllowedValue(newText);
                                    }
                                    c.add(newText);
                                    c.setSelection(new Point(0, newText
                                            .length()));
                                    currentSettingInSelection
                                            .setSettingValue(newText);
                                } catch (NumberFormatException ex) {
                                }
                            }
                        }
                    });
                }
            }

            return combo;
        }

        @Override
        protected Object getValue(Object element) {
            // This method returns the index of the item selected in the combo
            // box
            int index;
            AutomationSettingHelper setting = (AutomationSettingHelper) element;
            List<String> itemList = setting.getAllowedValues();
            String currentValue = setting.getSettingValue();
            index = itemList.indexOf(currentValue);
            return index;
        }

        @Override
        protected void setValue(Object element, Object value) {
            AutomationSettingHelper setting = (AutomationSettingHelper) element;
            int index = (Integer) value;
            List<String> valueList = setting.getAllowedValues();
            if (null != valueList) {
                String targetValue;
                if (index != -1) {
                    targetValue = valueList.get(index);
                    setting.setSettingValue(targetValue);

                    if (setting.getSettingID().equals(Constants.AUTOMATION)) {

                        if (attributeInSelection.isAutomationInProgress()) {
                            // settingsVisibility = false;
                            setVisibilityForManualValueChange(false);
                        } else {
                            if (targetValue.equals(Constants.ENABLE)) {
                                // settingsVisibility = true;
                                setVisibilityForManualValueChange(false);
                            } else {
                                // settingsVisibility = false;
                                setVisibilityForManualValueChange(true);
                            }
                        }

                        // If status is changed, then enable the submit and
                        // clear buttons
                        if (!targetValue.equals(Utility
                                .getAutomationString(attributeInSelection
                                        .isAutomationInProgress()))) {
                            setSettingSubmitVisibility(true);
                        } else {
                            setSettingSubmitVisibility(false);
                        }
                    }
                }
            }
            viewer.refresh();
        }
    }

    private boolean showSettings() {
        boolean show = false;
        if (!attributeInSelection.isAutomationInProgress()) {
            AutomationSettingHelper setting;
            Iterator<AutomationSettingHelper> list = localSettingList
                    .iterator();
            while (list.hasNext()) {
                setting = list.next();
                if (setting.getSettingID().equals(Constants.AUTOMATION)) {
                    if (setting.getSettingValue().equals(Constants.ENABLE)) {
                        show = true;
                    }
                    break;
                }
            }
        }
        return show;
    }

    // Disable the appropriate controls which are used for manual value change
    private void setVisibilityForManualValueChange(boolean visibility) {
        if (null != valueCmb && !valueCmb.isDisposed()) {
            valueCmb.setEnabled(visibility);
        }
        if (null != newValueLbl && !newValueLbl.isDisposed()) {
            newValueLbl.setEnabled(visibility);
        }
    }

    @Override
    public void dispose() {
        // Unregister the selection listener
        if (null != resourceSelectionChangedListener) {
            resourceManager
                    .removeResourceSelectionChangedUIListener(resourceSelectionChangedListener);
        }

        // Unregister the model change listener
        if (null != resourceModelChangedUIListener) {
            resourceManager
                    .removeResourceModelChangedUIListener(resourceModelChangedUIListener);
        }

        // Unregister the automation complete listener
        if (null != automationUIListener) {
            resourceManager.removeAutomationUIListener(automationUIListener);
        }

        super.dispose();
    }

    @Override
    public void setFocus() {

    }
}