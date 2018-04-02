/*
 *******************************************************************
 *
 * Copyright 2018 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.ObservationInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.Map;
import java.util.Observable;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * Resource
 *
 * This is the base class for all resources
 */

abstract public class Resource extends Observable implements OcPlatform.EntityHandler {
    static public final String RT_KEY = "rt";
    static public final String IF_KEY = "if";

    private String resUri;
    private String resType;
    private String resIf;
    private OcResourceHandle resHandle;
    private Map<Byte,Byte> observerIds = new ConcurrentHashMap<Byte,Byte>();

    public Resource(String uri, String resType, String resIf) {
        resUri = uri;
        this.resType = resType;
        this.resIf = resIf;

        try {
            resHandle = OcPlatform.registerResource(resUri, resType, resIf, this,
                    EnumSet.of(ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE, ResourceProperty.SECURE, ResourceProperty.SLOW));
            if (resHandle != null) {
                OcPlatform.bindInterfaceToResource(resHandle, OcPlatform.DEFAULT_INTERFACE);
            }

        } catch (OcException e) {
            OcfLightServer.msgError("Failed to create resource " + resUri);
            e.printStackTrace();
        }
    }

    @Override
    public synchronized EntityHandlerResult handleEntity(final OcResourceRequest request) {
        EntityHandlerResult ehResult = EntityHandlerResult.ERROR;
        if (null == request) {
            OcfLightServer.msg("Server request is invalid");
            return ehResult;
        }
        // Get the request flags
        EnumSet<RequestHandlerFlag> requestFlags = request.getRequestHandlerFlagSet();
        if (requestFlags.contains(RequestHandlerFlag.INIT)) {
            OcfLightServer.msg("Request Flag: Init for " + request.getResourceUri());
            ehResult = EntityHandlerResult.OK;
        }
        if (requestFlags.contains(RequestHandlerFlag.REQUEST)) {
            ehResult = handleRequest(request);
        }
        if (requestFlags.contains(RequestHandlerFlag.OBSERVER)) {
            ehResult = handleObserver(request);
        }
        return ehResult;
    }

    private EntityHandlerResult handleRequest(OcResourceRequest request) {
        EntityHandlerResult ehResult = EntityHandlerResult.ERROR;
        // Check for query params (if any)
        Map<String, String> queries = request.getQueryParameters();
        if (!queries.isEmpty()) {
            for (Map.Entry<String, String> entry : queries.entrySet()) {
                OcfLightServer.msg("Query key: " + entry.getKey() + " value: " + entry.getValue());
            }
        }

        // Get the request type
        RequestType requestType = request.getRequestType();
        switch (requestType) {
        case GET:
            ehResult = handleGetRequest(request);
            break;
        case PUT:
            ehResult = handlePutRequest(request);
            break;
        case POST:
            ehResult = handlePutRequest(request); // same as put
            break;
        case DELETE:
            ehResult = handleDeleteRequest();
            break;
        }
        return ehResult;
    }

    private EntityHandlerResult handleGetRequest(final OcResourceRequest request) {
        if (OcfLightServer.useSlowResponse()) {
            Random random = new Random();
            int delay = random.nextInt(10) + 1; // random int between 1 and 10 inclusive
            OcfLightServer.msg("Sending slow get response in " + delay + " seconds");

            ScheduledExecutorService slowResponseExecutor = new ScheduledThreadPoolExecutor(1);
            slowResponseExecutor.schedule(new Runnable() {
                public void run() {
                  OcResourceResponse response = new OcResourceResponse();
                  response.setRequestHandle(request.getRequestHandle());
                  response.setResourceHandle(request.getResourceHandle());
                  response.setResponseResult(EntityHandlerResult.OK);
                  response.setResourceRepresentation(getOcRepresentation());
                  sendResponse(response);
                }
            }, delay, TimeUnit.SECONDS);

            return EntityHandlerResult.SLOW;
        }

        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());
        response.setResponseResult(EntityHandlerResult.OK);
        response.setResourceRepresentation(getOcRepresentation());
        return sendResponse(response);
    }

    private EntityHandlerResult handlePutRequest(final OcResourceRequest request) {
        if (OcfLightServer.useSlowResponse()) {
            Random random = new Random();
            int delay = random.nextInt(10) + 1; // random int between 1 and 10 inclusive
            OcfLightServer.msg("Sending slow put response in " + delay + " seconds");

            ScheduledExecutorService slowResponseExecutor = new ScheduledThreadPoolExecutor(1);
            slowResponseExecutor.schedule(new Runnable() {
                public void run() {
                    OcResourceResponse response = new OcResourceResponse();
                    response.setRequestHandle(request.getRequestHandle());
                    response.setResourceHandle(request.getResourceHandle());

                    setOcRepresentation(request.getResourceRepresentation());
                    response.setResourceRepresentation(getOcRepresentation());
                    response.setResponseResult(EntityHandlerResult.OK);
                    sendResponse(response);

                    // notify ocf observers
                    notifyObservers(request);

                    // notify ui observers (ie implementors of Observer)
                    setChanged();
                    notifyObservers();
                }
            }, delay, TimeUnit.SECONDS);

            return EntityHandlerResult.SLOW;
        }

        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());

        setOcRepresentation(request.getResourceRepresentation());
        response.setResourceRepresentation(getOcRepresentation());
        response.setResponseResult(EntityHandlerResult.OK);

        // notify on separate thread
        ScheduledExecutorService observerNotifierExecutor = new ScheduledThreadPoolExecutor(1);
        observerNotifierExecutor.schedule(new Runnable() {
            public void run() {
                // notify ocf observers
                notifyObservers(request);

                // notify ui observers (ie implementors of Observer)
                setChanged();
                notifyObservers();
            }
        }, 200, TimeUnit.MILLISECONDS);

        return sendResponse(response);
    }

    private EntityHandlerResult handleDeleteRequest() {
        try {
            unregisterResource();
            return EntityHandlerResult.RESOURCE_DELETED;
        } catch (OcException e) {
            OcfLightServer.msgError(e.toString());
            OcfLightServer.msgError("Failed to unregister resource " + resUri);
            return EntityHandlerResult.ERROR;
        }
    }

    private EntityHandlerResult handleObserver(final OcResourceRequest request) {
        ObservationInfo observationInfo = request.getObservationInfo();
        switch (observationInfo.getObserveAction()) {
        case REGISTER:
            byte id = observationInfo.getOcObservationId();
            OcfLightServer.msg("handleObserver register observer " + String.format("%02x", id));
            observerIds.put(id, id);
            break;
        case UNREGISTER:
            OcfLightServer.msg("handleObserver unregister observer " + String.format("%02x", observationInfo.getOcObservationId()));
            observerIds.remove(observationInfo.getOcObservationId());
            break;
        }
        return OcfLightServer.useSlowResponse() ? EntityHandlerResult.SLOW : EntityHandlerResult.OK;
    }

    protected void notifyObservers(OcResourceRequest request) {
        if (!observerIds.isEmpty()) {
            try {
                OcResourceResponse response = new OcResourceResponse();
                response.setResourceRepresentation(getOcRepresentation());
                for (Byte id : observerIds.keySet()) {
                    OcfLightServer.msg("Notifying observer " + String.format("%02x", id) + " for resource " + this);
                }
                OcPlatform.notifyListOfObservers(getResourceHandle(), new ArrayList<Byte>(observerIds.keySet()), response);
            } catch (OcException e) {
                ErrorCode errorCode = e.getErrorCode();
                if (ErrorCode.NO_OBSERVERS == errorCode) {
                    OcfLightServer.msg("No observers found");
                } else {
                    OcfLightServer.msgError(e.toString());
                    OcfLightServer.msgError("Failed to notify observers");
                }
            }
        }
    }

    private EntityHandlerResult sendResponse(OcResourceResponse response) {
        try {
            OcPlatform.sendResponse(response);
            return EntityHandlerResult.OK;
        } catch (OcException e) {
            OcfLightServer.msgError(e.toString());
            OcfLightServer.msgError("Failed to send response");
            return EntityHandlerResult.ERROR;
        }
    }

    public synchronized void unregisterResource() throws OcException {
        if (null != resHandle) {
            OcPlatform.unregisterResource(resHandle);
            OcfLightServer.msg("Unregistered resource " + resUri);
        }
    }

    public abstract void setOcRepresentation(OcRepresentation rep);

    public abstract OcRepresentation getOcRepresentation();

    public String getResourceUri() {
        return resUri;
    }

    public OcResourceHandle getResourceHandle() {
        return resHandle;
    }

    public String[] getResourceTypeArray() {
        String[] rtArray = new String[1];
        rtArray[0] = resType;
        return rtArray;
    }

    public String[] getResourceInterfaceArray() {
        String[] ifArray = new String[2];
        ifArray[0] = OcPlatform.DEFAULT_INTERFACE;
        ifArray[1] = resIf;
        return ifArray;
    }

    @Override
    public String toString() {
        return resUri;
    }
}
