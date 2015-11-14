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

public class StartStopResourcePage extends WizardPage {

    private Button  startButton;

    private boolean start;

    protected StartStopResourcePage() {
        super("Start/Stop Resource");
        setStart(true);
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.START_STOP_PAGE_TITLE);
        setMessage(Constants.START_STOP_PAGE_MESSAGE);
        Composite compContent = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        compContent.setLayout(gridLayout);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        compContent.setLayoutData(gd);

        startButton = new Button(compContent, SWT.CHECK);
        startButton.setText("Start Resource(s)");
        startButton.setSelection(true);
        gd = new GridData();
        gd.verticalIndent = 10;
        startButton.setLayoutData(gd);

        Label descLbl = new Label(compContent, SWT.NONE);
        descLbl.setText("Description:");
        gd = new GridData();
        gd.verticalIndent = 50;
        descLbl.setLayoutData(gd);

        final Text text = new Text(compContent, SWT.MULTI | SWT.READ_ONLY
                | SWT.BORDER | SWT.WRAP | SWT.V_SCROLL);
        text.setText("Starting the resource will register it with the platform and makes it visible on the network.\n"
                + "Resources can be started or stopped later from 'Resource Manager' view.");
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.heightHint = 100;
        gd.verticalAlignment = SWT.BOTTOM;
        text.setLayoutData(gd);

        addUIListeners();

        setControl(compContent);
    }

    private void addUIListeners() {
        startButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setStart(startButton.getSelection());
            }
        });
    }

    public boolean isStart() {
        return start;
    }

    public void setStart(boolean start) {
        this.start = start;
    }

    @Override
    public IWizardPage getNextPage() {
        System.out.println(this.getClass().getName() + ": getNextPage");
        return null;
    }
}
