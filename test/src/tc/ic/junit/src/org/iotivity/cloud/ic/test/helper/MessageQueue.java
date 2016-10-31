package org.iotivity.cloud.ic.test.helper;

import java.util.List;
import java.util.EnumSet;

import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatform.*;
import org.iotivity.base.OcException;
import org.iotivity.base.OcResource.OnObserveListener;
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

import android.util.Log;
import android.content.Context;
import java.lang.*;


public class MessageQueue implements OnPostListener, OnGetListener,
		OnDeleteListener, OnObserveListener {
	
	private boolean             isResourceFound                    = false;
	private OcResource          resource;
	private static final String RT                                 = "?rt=";

	public void createResource(Context context) {
		
		PlatformConfig cfg = new PlatformConfig(context, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0,
                QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        
		OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {
			@Override
			public EntityHandlerResult handleEntity(
					OcResourceRequest ocResourceRequest) {
				return EntityHandlerResult.OK;
			}
		};

		OcResourceHandle handler = null;

		try {
			handler = OcPlatform.registerResource("/a/light", "core.light",
					"oic.if.baseline", entityHandler,
					EnumSet.of(ResourceProperty.DISCOVERABLE));
		} catch (OcException ex) {
		}
	}
	
	OcPlatform.OnResourceFoundListener listener = new OcPlatform.OnResourceFoundListener() {

        @Override
        public void onFindResourceFailed(Throwable throwable, String s) {
            isResourceFound = true;
        }

        @Override
        public void onResourceFound(OcResource ocResource) {
            isResourceFound = true;
            if (ocResource == null) {
            } else {
                resource = ocResource;
                Log.d("Resource found", ocResource.getUri());
            }
            isResourceFound = true;
        }

    };

    public OcResource findClintResource(Context context) {
    	String requestUri = OcPlatform.WELL_KNOWN_QUERY + RT + "core.light";
    	
        createResource(context);
        
        try {
            OcPlatform.findResource("", requestUri,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), listener);
            
            try {
            	Thread.sleep(5000);
            } catch (InterruptedException ex) {
                Log.d("Wait not work", ex.getMessage());
            }
        } catch (OcException ex) {
            Log.d("Resource found", ex.getMessage());
        }
        return resource;
    }

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
