/*
 *******************************************************************
 *
 * Copyright 2017 Intel Corporation.
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
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;

import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * ServerLight
 * <p/>
 * This class represents a server light resource.
 */
public class ServerLight extends SimpleLight implements OcPlatform.EntityHandler {

    private static final String TAG = ServerLight.class.getSimpleName();

    public static final String DEFAULT_RESOURCE_URI = "/a/light";
    public static final String DEFAULT_RESOURCE_NAME = "John's light";

    private SimpleServer mServer; // server
    private String mResourceUri; // resource URI
    private String mResourceTypeName; // resource type name
    private String mResourceInterface; // resource interface
    private OcResourceHandle mResourceHandle; // resource handle

    private Thread mObserverNotifier;

    public ServerLight(SimpleServer server, String resourceUri, String name, boolean state, int power) {
        mServer = server;
        mResourceUri = resourceUri;
        mResourceTypeName = SimpleLight.RESOURCE_TYPE;
        mResourceInterface = OcPlatform.DEFAULT_INTERFACE;
        mResourceHandle = null; // this is set when resource is registered

        setName(name);
        setState(state);
        setPower(power);
    }

    public synchronized void registerResource() throws OcException {
        if (null == mResourceHandle) {
            mResourceHandle = OcPlatform.registerResource(mResourceUri, mResourceTypeName, mResourceInterface, this,
                    EnumSet.of(ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE));
        }
    }

    /**
     * NOTE: This is just a sample implementation of entity handler. Entity
     * handler can be implemented in several ways by the manufacturer.
     */
    @Override
    public synchronized EntityHandlerResult handleEntity(final OcResourceRequest request) {
        EntityHandlerResult ehResult = EntityHandlerResult.ERROR;
        if (null == request) {
            msg("Server request is invalid");
            return ehResult;
        }
        // Get the request flags
        EnumSet<RequestHandlerFlag> requestFlags = request.getRequestHandlerFlagSet();
        if (requestFlags.contains(RequestHandlerFlag.INIT)) {
            msg("\t\tRequest Flag: Init");
            ehResult = EntityHandlerResult.OK;
        }
        if (requestFlags.contains(RequestHandlerFlag.REQUEST)) {
            msg("\t\tRequest Flag: Request");
            ehResult = handleRequest(request);
        }
        if (requestFlags.contains(RequestHandlerFlag.OBSERVER)) {
            msg("\t\tRequest Flag: Observer");
            ehResult = handleObserver(request);
        }
        return ehResult;
    }

    private EntityHandlerResult handleRequest(OcResourceRequest request) {
        EntityHandlerResult ehResult = EntityHandlerResult.ERROR;
        // Check for query params (if any)
        Map<String, String> queries = request.getQueryParameters();
        if (!queries.isEmpty()) {
            msg("Query processing is up to entityHandler");
        } else {
            msg("No query parameters in this request");
        }

        for (Map.Entry<String, String> entry : queries.entrySet()) {
            msg("Query key: " + entry.getKey() + " value: " + entry.getValue());
        }

        // Get the request type
        RequestType requestType = request.getRequestType();
        switch (requestType) {
        case GET:
            msg("\t\t\tRequest Type is GET");
            ehResult = handleGetRequest(request);
            break;
        case PUT:
            msg("\t\t\tRequest Type is PUT");
            ehResult = handlePutRequest(request);
            break;
        case POST:
            msg("\t\t\tRequest Type is POST");
            ehResult = handlePostRequest(request);
            break;
        case DELETE:
            msg("\t\t\tRequest Type is DELETE");
            ehResult = handleDeleteRequest();
            break;
        }
        return ehResult;
    }

    private EntityHandlerResult handleGetRequest(final OcResourceRequest request) {
        EntityHandlerResult ehResult;
        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());

        response.setResponseResult(EntityHandlerResult.OK);
        response.setResourceRepresentation(getOcRepresentation());
        ehResult = sendResponse(response);
        return ehResult;
    }

    private EntityHandlerResult handlePutRequest(OcResourceRequest request) {
        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());

        setOcRepresentation(request.getResourceRepresentation());
        response.setResourceRepresentation(getOcRepresentation());
        response.setResponseResult(EntityHandlerResult.OK);
        return sendResponse(response);
    }

    private int sUriCounter = 1;

    private EntityHandlerResult handlePostRequest(OcResourceRequest request) {
        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());
        String newUri = DEFAULT_RESOURCE_URI + "/" + (++sUriCounter);
        mServer.createNewLightResource(newUri, DEFAULT_RESOURCE_NAME + " " + sUriCounter);
        OcRepresentation rep_post = getOcRepresentation();
        try {
            rep_post.setValue(OcResource.CREATED_URI_KEY, newUri);
        } catch (OcException e) {
            msgError(TAG, e.toString());
        }
        response.setResourceRepresentation(rep_post);
        response.setNewResourceUri(newUri);
        response.setResponseResult(EntityHandlerResult.RESOURCE_CREATED);
        return sendResponse(response);
    }

    private EntityHandlerResult handleDeleteRequest() {
        try {
            unregisterResource();
            return EntityHandlerResult.RESOURCE_DELETED;
        } catch (OcException e) {
            msgError(TAG, e.toString());
            msg("Failed to unregister a light resource");
            return EntityHandlerResult.ERROR;
        }
    }

    private List<Byte> mObservationIds = new LinkedList<>();

    private EntityHandlerResult handleObserver(final OcResourceRequest request) {
        ObservationInfo observationInfo = request.getObservationInfo();
        switch (observationInfo.getObserveAction()) {
        case REGISTER:
            mObservationIds.add(observationInfo.getOcObservationId());
            break;
        case UNREGISTER:
            mObservationIds.remove((Byte) observationInfo.getOcObservationId());
            break;
        }
        // Observation happens on a different thread in notifyObservers method.
        // If we have not created the thread already, we will create one here.
        if (null == mObserverNotifier) {
            mObserverNotifier = new Thread(new Runnable() {
                public void run() {
                    notifyObservers(request);
                }
            });
            mObserverNotifier.start();
        }
        return EntityHandlerResult.OK;
    }

    private void notifyObservers(OcResourceRequest request) {
        while (true) {
            // increment current power value by 10 every 2 seconds
            setPower(getPower() + 10);
            sleep(2);

            msg("Notifying observers...");
            msg(toString());
            try {
                OcPlatform.notifyAllObservers(mResourceHandle);
            } catch (OcException e) {
                ErrorCode errorCode = e.getErrorCode();
                if (ErrorCode.NO_OBSERVERS == errorCode) {
                    msg("No more observers, stopping notifications");
                }
                return;
            }
        }
    }

    private EntityHandlerResult sendResponse(OcResourceResponse response) {
        try {
            OcPlatform.sendResponse(response);
            return EntityHandlerResult.OK;
        } catch (OcException e) {
            msgError(TAG, e.toString());
            msg("Failed to send response");
            return EntityHandlerResult.ERROR;
        }
    }

    public synchronized void unregisterResource() throws OcException {
        if (null != mResourceHandle) {
            OcPlatform.unregisterResource(mResourceHandle);
        }
    }

    public void setOcRepresentation(OcRepresentation rep) {
        try {
            if (rep.hasAttribute(NAME_KEY))
                setName((String) rep.getValue(NAME_KEY));
            if (rep.hasAttribute(STATE_KEY))
                setState((boolean) rep.getValue(STATE_KEY));
            if (rep.hasAttribute(POWER_KEY))
                setPower((int) rep.getValue(POWER_KEY));
        } catch (OcException e) {
            msgError(TAG, e.toString());
            msg("Failed to get representation values");
        }
    }

    public OcRepresentation getOcRepresentation() {
        OcRepresentation rep = new OcRepresentation();
        try {
            rep.setValue(NAME_KEY, getName());
            rep.setValue(STATE_KEY, getState());
            rep.setValue(POWER_KEY, getPower());
        } catch (OcException e) {
            msgError(TAG, e.toString());
            msg("Failed to set representation values");
        }
        return rep;
    }

    // ******************************************************************************
    // End of the OIC specific code
    // ******************************************************************************

    @Override
    public String toString() {
        return "\t" + "URI" + ": " + mResourceUri + "\n\t" + NAME_KEY + ": " + getName() + "\n\t" + STATE_KEY + ": "
                + getState() + "\n\t" + POWER_KEY + ": " + getPower();
    }

    private void sleep(int seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
            msgError(TAG, e.toString());
        }
    }

    private void msg(String text) {
        mServer.msg(text);
    }

    private void msgError(String tag, String text) {
        mServer.msgError(tag, text);
    }
}
