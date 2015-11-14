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

public class SimpleResourcePage extends WizardPage {

    Button manualBtn;
    Button ramlBtn;

    protected SimpleResourcePage() {
        super("Simle Page");
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle(Constants.SIMPLE_PAGE_TITLE);
        setMessage(Constants.SIMPLE_PAGE_MESSAGE);
        Composite compContent = new Composite(parent, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        compContent.setLayout(gridLayout);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        compContent.setLayoutData(gd);

        manualBtn = new Button(compContent, SWT.RADIO);
        manualBtn.setText("Manually");

        ramlBtn = new Button(compContent, SWT.RADIO);
        ramlBtn.setText("From RAML file");

        Label lbl = new Label(compContent, SWT.NULL);
        lbl.setText("Details:");
        gd = new GridData();
        gd.verticalIndent = 20;
        lbl.setLayoutData(gd);

        Text desc = new Text(compContent, SWT.MULTI | SWT.READ_ONLY
                | SWT.BORDER | SWT.WRAP | SWT.V_SCROLL);
        desc.setBackground(compContent.getBackground());
        desc.setText("[Select an option to view more details]");
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        desc.setLayoutData(gd);

        addUIListeners();

        setControl(compContent);
    }

    private void addUIListeners() {
        manualBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                getWizard().getContainer().updateButtons();
            }
        });

        ramlBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                getWizard().getContainer().updateButtons();
            }
        });
    }

    @Override
    public boolean canFlipToNextPage() {
        if (manualBtn.getSelection() || ramlBtn.getSelection()) {
            return true;
        }
        return false;
    }

    @Override
    public IWizardPage getNextPage() {
        CreateResourceWizard createWizard = (CreateResourceWizard) getWizard();
        if (manualBtn.getSelection()) {
            return createWizard.getSimpleResourceBasicDetailsPage();
        }
        if (ramlBtn.getSelection()) {
            return createWizard.getLoadRamlPage();
        }
        return null;
    }
}
