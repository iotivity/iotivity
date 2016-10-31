package org.iotivity.cloud.ic.test.helper;

import java.util.List;

import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;

public class ICResourceDirectoryCommonAdapter implements
        OcPlatform.OnPublishResourceListener,
        OcPlatform.OnDeleteResourceListener, OcResource.OnObserveListener {

    public ICResourceDirectoryCommonAdapter() {

    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> arg0,
            OcRepresentation arg1, int arg2) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onObserveFailed(Throwable arg0) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onDeleteResourceCompleted(int arg0) {
        // TODO Auto-generated method stub
        
    }

    public void onDeleteResourceFailed(Throwable arg0) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onPublishResourceCompleted(OcRepresentation arg0) {
        // TODO Auto-generated method stub
        
    }

    @Override
    public void onPublishResourceFailed(Throwable arg0) {
        // TODO Auto-generated method stub
        
    }
    
}
