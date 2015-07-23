package oic.simulator.serviceprovider.view.dialogs;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.utils.Convertion;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

public class DeleteResourceWizard extends Wizard {

    private DeleteResourcePage page;

    public DeleteResourceWizard() {
        setWindowTitle("Delete resources");
    }

    @Override
    public void addPages() {
        page = new DeleteResourcePage();
        addPage(page);
    }

    @Override
    public boolean performFinish() {
        // Check the existence of the resource if the user has entered the uri
        if (page.getDeleteCategory() == DeleteCategory.BY_URI) {
            // Check whether the uri is in full form or short form
            // If it is in short form, expand it to its full form.
            String uri = page.getDeleteCandidate();
            boolean uriComplete = Convertion.isUriComplete(uri);
            if (!uriComplete) {
                uri = Convertion.displayNameToUri(uri);
            }
            boolean exist = Activator.getManager().isResourceExist(uri);
            if (!exist) {
                Shell activeShell = PlatformUI.getWorkbench().getDisplay()
                        .getActiveShell();
                MessageDialog dialog = new MessageDialog(activeShell,
                        "Resource Not Found", null,
                        "No resource exist with the given URI.",
                        MessageDialog.INFORMATION, new String[] { "OK" }, 0);
                dialog.open();
                page.setFocusToTextBox();
                return false;
            }
        }
        return true;
    }

    public DeleteCategory getDeleteCategory() {
        return page.getDeleteCategory();
    }

    public String getDeleteCandidate() {
        return page.getDeleteCandidate();
    }
}
