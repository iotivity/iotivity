package oic.simulator.serviceprovider.view.dialogs;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Map;

import oic.simulator.serviceprovider.manager.LogManager;

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

public class FilterDialog extends TrayDialog {
    private Map<Integer, Boolean> severities;

    public FilterDialog(Shell shell, Map<Integer, Boolean> severities) {
        super(shell);
        this.severities = severities;
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite composite = (Composite) super.createDialogArea(parent);
        createSeverityGroup(composite);
        getShell().setText("Filter details");
        return composite;
    }

    /**
     * Dynamically creates a check-box list for severity levels for user to
     * choose from
     */
    private void createSeverityGroup(Composite parent) {
        Group group = new Group(parent, SWT.NONE);
        group.setLayout(new GridLayout());
        GridData gd = new GridData(GridData.FILL_HORIZONTAL);
        gd.widthHint = 275;
        group.setLayoutData(gd);
        group.setText("Severity Levels");

        ArrayList<Integer> arrayList = new ArrayList<Integer>(
                severities.keySet());
        Collections.sort(arrayList);
        for (final Integer i : arrayList) {
            final Button checkbox = new Button(group, SWT.CHECK);
            checkbox.setText(LogManager.getSeverityName(i));
            checkbox.setSelection(severities.get(i));
            checkbox.addSelectionListener(new SelectionAdapter() {

                @Override
                public void widgetSelected(SelectionEvent e) {
                    severities.put(i, checkbox.getSelection());
                }
            });
        }
    }

    @Override
    public boolean isHelpAvailable() {
        return false;
    }
}