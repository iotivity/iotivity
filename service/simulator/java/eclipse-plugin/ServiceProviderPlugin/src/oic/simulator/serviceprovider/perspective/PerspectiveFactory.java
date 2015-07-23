package oic.simulator.serviceprovider.perspective;

import oic.simulator.serviceprovider.view.AttributeView;
import oic.simulator.serviceprovider.view.LogView;
import oic.simulator.serviceprovider.view.MultiResourceOrchestrationView;
import oic.simulator.serviceprovider.view.ResourceManagerView;

import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;

public class PerspectiveFactory implements IPerspectiveFactory {

    public static final String PERSPECTIVE_ID = "oic.simulator.serviceprovider.perspective";
    private IPageLayout        factory;

    @Override
    public void createInitialLayout(IPageLayout factory) {
        this.factory = factory;
        factory.setEditorAreaVisible(false);
        addViews();
        factory.setFixed(false);
    }

    private void addViews() {
        factory.addView(ResourceManagerView.VIEW_ID, IPageLayout.LEFT, 0.3f,
                factory.getEditorArea());
        factory.addView(IPageLayout.ID_PROP_SHEET, IPageLayout.BOTTOM, 0.6f,
                ResourceManagerView.VIEW_ID);
        factory.addView(AttributeView.VIEW_ID, IPageLayout.LEFT, 0.5f,
                factory.getEditorArea());
        factory.addView(LogView.VIEW_ID, IPageLayout.BOTTOM, 0.6f,
                AttributeView.VIEW_ID);
        factory.addView(MultiResourceOrchestrationView.VIEW_ID,
                IPageLayout.RIGHT, 0.5f, AttributeView.VIEW_ID);
    }
}
