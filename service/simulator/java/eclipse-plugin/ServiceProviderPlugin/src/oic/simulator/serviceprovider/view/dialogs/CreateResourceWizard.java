package oic.simulator.serviceprovider.view.dialogs;

import org.eclipse.jface.wizard.Wizard;

public class CreateResourceWizard extends Wizard {

    private CreateResourcePage page;

    public CreateResourceWizard() {
        setWindowTitle("Create resources");
    }

    @Override
    public void addPages() {
        page = new CreateResourcePage();
        addPage(page);
    }

    public String getConfigFilePath() {
        return page.getConfigFilePath();
    }

    public int getResourceCount() {
        return page.getResourceCount();
    }

    @Override
    public boolean performFinish() {
        return true;
    }

}
