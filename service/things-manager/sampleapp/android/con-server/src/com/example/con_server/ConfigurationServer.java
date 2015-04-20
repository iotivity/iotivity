/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
package com.example.con_server;

import java.util.EnumSet;
import java.util.Vector;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.service.tm.IConfigurationListener;
import org.iotivity.service.tm.IDiagnosticsListener;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.ThingsManager;

import android.os.Message;
import android.util.Log;

/*
 * For Creating the Resources [configurtion, Diagnostic & FactoryRest]  &
 * for Handling of the Client's Request
 */
public class ConfigurationServer implements IDiagnosticsListener,
        IConfigurationListener, OcPlatform.EntityHandler {
    private final String          LOG_TAG            = "[CON-SERVER]"
                                                             + this.getClass()
                                                                     .getSimpleName();
    private ThingsManager         thingsmanager      = null;
    private ConfigurationResource conResource        = null;
    private DiagnosticsResource   diagResource       = null;
    private FactorySetResource    factorySetResource = null;

    // constructor
    public ConfigurationServer() {
        thingsmanager = new ThingsManager();
        thingsmanager.setDiagnosticsListener(this);
        thingsmanager.setConfigurationListener(this);
    }

    public void DoBootStrap() {
        Log.i(LOG_TAG, "DoBootStrap: enter");

        OCStackResult result = thingsmanager.doBootstrap();
        if (OCStackResult.OC_STACK_ERROR == result) {
            Log.e(LOG_TAG, "doBootStrap returned error: "
                    + OCStackResult.OC_STACK_ERROR.name());
        }
        Log.i(LOG_TAG, "DoBootStrap: exit");
    }

    // Creating resources : configuration, diagnostics, factoryReset
    public void CreateConfigurationResource() {
        Log.i(LOG_TAG, "CreateConfigurationResource: enter");

        try {
            conResource = new ConfigurationResource();
            conResource.createResource(this);

            diagResource = new DiagnosticsResource();
            diagResource.createResource(this);

            factorySetResource = new FactorySetResource();
            factorySetResource.createResource(this);
        } catch (OcException e) {
            Log.e(LOG_TAG, "OcException occured: " + e.toString());
        }

        Log.i(LOG_TAG, "CreateConfigurationResource: exit");

        String message = "Resources Created Successfully(Server is Ready)";

        Message msg = Message.obtain();
        msg.what = 0;
        MainActivity mainActivityObj = MainActivity.getMainActivityObject();
        MainActivity.setmessage(message);
        mainActivityObj.getmHandler().sendMessage(msg);

    }

    // For deleting all the resources
    public void deleteResources() {
        if (null != conResource)
            conResource.deleteResource();
        if (null != diagResource)
            diagResource.deleteResource();
        if (null != factorySetResource)
            factorySetResource.deleteResource();
    }

    // Callback Function for doBootStrap
    @Override
    public void onBootStrapCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        String message;
        Log.i(LOG_TAG, "onBootStrapCallback");

        // setting the default values received from bootstrap Server

        ConfigurationDefaultValues.defaultRegion = rep.getValueString("r");
        ConfigurationDefaultValues.defaultSystemTime = rep.getValueString("st");
        ConfigurationDefaultValues.defaultCurrency = rep.getValueString("c");
        ConfigurationDefaultValues.defaultLocation = rep.getValueString("loc");

        // forming the message to display on UI
        message = "URI : " + rep.getUri() + "\n";
        message = message + "Region : "
                + ConfigurationDefaultValues.defaultRegion + "\n";
        message = message + "System Time : "
                + ConfigurationDefaultValues.defaultSystemTime + "\n";
        message = message + "Currency : "
                + ConfigurationDefaultValues.defaultCurrency + "\n";
        message = message + "Location : "
                + ConfigurationDefaultValues.defaultLocation + "\n";

        Log.i(LOG_TAG, "Resource URI: " + rep.getUri());
        Log.i(LOG_TAG, "Region: " + ConfigurationDefaultValues.defaultRegion);
        Log.i(LOG_TAG, "System Time: "
                + ConfigurationDefaultValues.defaultSystemTime);
        Log.i(LOG_TAG, "Currency: "
                + ConfigurationDefaultValues.defaultCurrency);
        Log.i(LOG_TAG, "Location: "
                + ConfigurationDefaultValues.defaultLocation);

        // showing the formed message on the UI
        Message msg = Message.obtain();
        msg.what = 0;
        MainActivity mainActivityObj = MainActivity.getMainActivityObject();
        MainActivity.setmessage(message);
        mainActivityObj.getmHandler().sendMessage(msg);
    }

    // Callback Function for Reboot
    @Override
    public void onRebootCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        Log.i(LOG_TAG, "onRebootCallback");
    }

    // Callback Function for FactoryReset
    @Override
    public void onFactoryResetCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        Log.i(LOG_TAG, "onFactoryResetCallback");
    }

    // For Handling the Client's Request
    @Override
    public EntityHandlerResult handleEntity(OcResourceRequest request) {
        Log.i(LOG_TAG, "handleEntity: enter");

        EntityHandlerResult result = EntityHandlerResult.ERROR;
        if (null == request) {
            Log.e(LOG_TAG, "handleEntity: Invalid OcResourceRequest!");
            return result;
        }

        RequestType requestType = request.getRequestType();
        EnumSet<RequestHandlerFlag> requestHandlerFlag = request
                .getRequestHandlerFlagSet();
        Log.i(LOG_TAG, "prepareResponseForResource: request type: "
                + requestType.name());
        Log.i(LOG_TAG, "prepareResponseForResource: request for resource: "
                + request.getResourceUri());

        if (requestHandlerFlag.contains(RequestHandlerFlag.REQUEST)) {
            if (RequestType.GET == requestType) {
                sendResponse(request);
            } else if (RequestType.PUT == requestType) {
                OcRepresentation rep = request.getResourceRepresentation();
                if (null == rep) {
                    Log.e(LOG_TAG,
                            "handleEntity: Invalid resource representation!");
                    return result;
                }

                if (request.getResourceUri().equalsIgnoreCase(
                        conResource.getUri())) {
                    conResource.setConfigurationRepresentation(rep);
                } else if (request.getResourceUri().equalsIgnoreCase(
                        diagResource.getUri())) {

                    String factorySetAtt = rep.getValueString("fr");
                    if (factorySetAtt.equalsIgnoreCase("true")) {
                        conResource.factoryReset();
                    }
                    diagResource.setDiagnosticsRepresentation(rep);
                }
                sendResponse(request);
            }
        }

        Log.i(LOG_TAG, "handleEntity: exit");
        return result;
    }

    // For sending response to the client
    private void sendResponse(OcResourceRequest request) {
        Log.i(LOG_TAG, "sendResponse: enter");

        OcResourceResponse response = new OcResourceResponse();
        OcRepresentation rep = null;

        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());

        if (request.getResourceUri().equalsIgnoreCase(conResource.getUri())) {
            rep = conResource.getConfigurationRepresentation();
        } else if (request.getResourceUri().equalsIgnoreCase(
                diagResource.getUri())) {
            rep = diagResource.getDiagnosticsRepresentation();
        }
        response.setResourceRepresentation(rep, OcPlatform.DEFAULT_INTERFACE);
        response.setErrorCode(200);

        try {
            OcPlatform.sendResponse(response);
        } catch (OcException e) {
            Log.e(LOG_TAG, "sendResponse: OcException occured: " + e.toString());
        }
        Log.i(LOG_TAG, "sendResponse: exit");
    }

    @Override
    public void onUpdateConfigurationsCallback(
            Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
            int errorValue) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onGetConfigurationsCallback(
            Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
            int errorValue) {
        // TODO Auto-generated method stub

    }
}

// Default values for Resources
class ConfigurationDefaultValues {

    // configuration Resource default values
    public static String defaultLocation        = new String();
    public static String defaultRegion          = new String();
    public static String defaultSystemTime      = new String();
    public static String defaultCurrency        = new String();
    public static String ConURIPrefix           = "/oic/con";
    public static String ConResourceTypePrefix  = "oic.con";

    // Diagnostics Resource default values
    public static String diagURIPrefix          = "/oic/diag";
    public static String diagResourceTypePrefix = "oic.diag";
    public static String diagnosticsValue       = "false";
    public static String defaultFactoryReset    = "false";
    public static String defaultReboot          = "false";
    public static String defaultStartCollection = "false";
}
