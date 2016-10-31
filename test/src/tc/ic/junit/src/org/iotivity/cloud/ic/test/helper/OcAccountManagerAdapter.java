package org.iotivity.cloud.ic.test.helper;

import java.util.List;

import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcException;
import org.iotivity.base.OcAccountManager.OnObserveListener;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;
import org.iotivity.base.ModeType;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcAccountManager.OnPostListener;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;

public class OcAccountManagerAdapter implements OnPostListener, OnGetListener, OnDeleteListener, OnObserveListener{

	@Override
	public void onPostCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onPostFailed(Throwable arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onGetCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onGetFailed(Throwable arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onDeleteCompleted(List<OcHeaderOption> arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onDeleteFailed(Throwable arg0) {
		// TODO Auto-generated method stub
		
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
}
