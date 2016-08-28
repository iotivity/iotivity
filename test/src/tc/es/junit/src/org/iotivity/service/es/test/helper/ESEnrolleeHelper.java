/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.es.test.helper;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcException;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;
import org.iotivity.base.ModeType;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.OcResourceHandle;

import android.util.Log;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

import android.content.Context;

public class ESEnrolleeHelper {
    private static final String ALL_INTERFACE_TYPE                 = "0.0.0.0";
    private static final String EMPTY_STRING                       = "";
    private static final String FOUND_RESOURCE_IS_INVALID          = "Found resource is invalid";
    private static final String FAIL_TO_FIND_RESOURCE              = "Fail to find resource";
    private static final String FAIL_TO_REGISTER_ENROLLEE_RESOURCE = "Fail to register enrollee resource";
    private static final String INTERFACE_TYPE_OF_THE_RESOURCE     = "Interface Type of the resource : ";
    private static final String RESOURCE_TYPE_OF_THE_RESOURCE      = "Resource Type of the resource : ";
    private static final String HOST_ADDRESS_OF_THE_RESOURCE       = "Host address of the resource: ";
    private static final String URI_OF_THE_RESOURCE                = "URI of the resource: ";
    private static final String ENROLLEEOUND                       = "Enrolleeound";
    private static final String RT                                 = "?rt=";
    Map<String, String>         queryParamsMap                     = new HashMap<String, String>();
    private static final String TAG                                = "Enrollee Resource Creation: ";
    private boolean             isResourceFound                    = false;
    private OcResource          enrolleeResource;
    private static final String RESOURCE_NAME                      = "/a/light";
    private static final String PROV_RESOURCE_TYPE                 = "ocf.wk.prov";
    private static final String BATCH_INTERFACE_TYPE               = OcPlatform.BATCH_INTERFACE;

    public ESEnrolleeHelper() {
        enrolleeResource = null;
        isResourceFound = false;
    }

    private boolean configurePlatform(Context context) {
        PlatformConfig cfg = new PlatformConfig(context, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, ALL_INTERFACE_TYPE, 0,
                QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        return true;
    }

    private boolean registerEnrolleeResource(String resourceName,
            String resourceType, String interfaceType) {

        OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(
                    OcResourceRequest ocResourceRequest) {
                return EntityHandlerResult.OK;
            }
        };

        OcResourceHandle handler = null;

        try {
            handler = OcPlatform.registerResource(resourceName, resourceType,
                    interfaceType, entityHandler,
                    EnumSet.of(ResourceProperty.DISCOVERABLE));
        } catch (OcException ex) {
            Log.d(TAG, ex.getMessage());
        }

        if (handler == null) {
            Log.d(TAG, FAIL_TO_REGISTER_ENROLLEE_RESOURCE);
            return false;
        }

        return true;
    }

    OcPlatform.OnResourceFoundListener listener = new OcPlatform.OnResourceFoundListener() {

        @Override
        public void onFindResourceFailed(Throwable throwable, String s) {
            Log.d(TAG, FAIL_TO_FIND_RESOURCE);
            isResourceFound = true;
        }

        @Override
        public void onResourceFound(OcResource ocResource) {
            if (ocResource == null) {
                Log.d(TAG, FOUND_RESOURCE_IS_INVALID);
            } else {
                enrolleeResource = ocResource;
                Log.d(TAG, ENROLLEEOUND);
                Log.d(TAG, URI_OF_THE_RESOURCE + ocResource.getUri());
                Log.d(TAG, HOST_ADDRESS_OF_THE_RESOURCE + ocResource.getHost());
                Log.d(TAG, RESOURCE_TYPE_OF_THE_RESOURCE
                        + ocResource.getResourceTypes().get(0));
                Log.d(TAG, INTERFACE_TYPE_OF_THE_RESOURCE
                        + ocResource.getResourceInterfaces().get(1));
            }
            isResourceFound = true;
        }

    };

    private OcResource findEnrolleeResource(String resourceType) {

        String requestUri = OcPlatform.WELL_KNOWN_QUERY + RT + resourceType;

        try {
            OcPlatform.findResource(EMPTY_STRING, requestUri,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), listener);
            ESUtility.waitUntilEnrolleeResponse(isResourceFound);
        } catch (OcException ex) {
            Log.d(TAG, ex.getMessage());
        }
        return null;
    }

    public OcResource findEnrolleeResource(Context context) {
        String requestUri = OcPlatform.WELL_KNOWN_QUERY + RT
                + PROV_RESOURCE_TYPE;
        configurePlatform(context);
        try {
            OcPlatform.findResource(EMPTY_STRING, requestUri,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), listener);
            ESUtility.waitUntilEnrolleeResponse(isResourceFound);
        } catch (OcException ex) {
            Log.d(TAG, ex.getMessage());
        }
        return enrolleeResource;
    }

    public OcResource createEnrolleeResource(Context context,
            String resourceName, String resourceType, String interfaceType) {

        if (configurePlatform(context) && registerEnrolleeResource(resourceName,
                resourceType, interfaceType)) {
            findEnrolleeResource(resourceType);
        }

        return enrolleeResource;
    }

    public RemoteEnrollee createRemoteEnrollee(Context context) {
        OcResource ocResource = createEnrolleeResource(context, RESOURCE_NAME,
                PROV_RESOURCE_TYPE, BATCH_INTERFACE_TYPE);
        if (ocResource == null) {
            return null;
        }
        return EasySetup.getInstance(context).createRemoteEnrollee(ocResource);
    }
}
