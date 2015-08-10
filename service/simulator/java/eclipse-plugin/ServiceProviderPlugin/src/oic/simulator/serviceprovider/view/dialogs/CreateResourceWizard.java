package oic.simulator.serviceprovider.view.dialogs;

import java.net.URL;

import oic.simulator.serviceprovider.Activator;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.wizard.Wizard;

public class CreateResourceWizard extends Wizard {

    private CreateResourcePage page;

    public CreateResourceWizard() {
        setWindowTitle("Create resources");
        IPath path = new Path("/icons/oic_logo_64x64.png");
        URL find = FileLocator.find(Activator.getDefault().getBundle(), path,
                null);
        setDefaultPageImageDescriptor(ImageDescriptor.createFromURL(find));
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