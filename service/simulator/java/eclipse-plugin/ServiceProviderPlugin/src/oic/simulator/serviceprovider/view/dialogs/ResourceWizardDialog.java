package oic.simulator.serviceprovider.view.dialogs;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.wizard.IWizard;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;

public class ResourceWizardDialog extends WizardDialog {

    public ResourceWizardDialog(Shell parentShell, IWizard newWizard) {
        super(parentShell, newWizard);
    }

    @Override
    public boolean isHelpAvailable() {
        return false;
    }

    @Override
    public void finishPressed() {
        super.finishPressed();
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        super.createButtonsForButtonBar(parent);
        Button finishButton = getButton(IDialogConstants.FINISH_ID);
        if (finishButton != null) {
            finishButton.setText(IDialogConstants.OK_LABEL);
        }
    }

}
