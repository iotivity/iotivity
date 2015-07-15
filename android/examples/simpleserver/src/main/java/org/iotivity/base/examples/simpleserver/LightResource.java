/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples.simpleserver;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import org.iotivity.base.EntityHandlerResult;
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

import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * LightResource
 *
 * class LightResource is the main class of the simpleServer. This creates resources and handles incoming requests from the client.
 */
public class LightResource implements IMessageLogger {
    private Context mContext;
    private static String TAG = "SimpleServer: ";

    private String mName;
    private Boolean mState;
    private Integer mPower;
    private OcRepresentation mLightRep;
    private OcResourceHandle mResourceHandle;
    private List<Byte> mObservationIds;
    private static boolean doOnce; // used in POST
    LightRepThread lightRepThread;

    // constructor
    LightResource(Context context) {
        mContext = context;
        mName = "John's light";
        mState = false;
        mPower = 0;
        mObservationIds = new LinkedList<>();
        mLightRep = new OcRepresentation();
        try {
            mLightRep.setValue(StringConstants.STATE, mState);
            mLightRep.setValue(StringConstants.POWER, mPower);
            mLightRep.setValue(StringConstants.NAME, mName);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        doOnce = true; // used in post
        lightRepThread = null;
    }

    //accessor methods
    protected int getPower() {
        return mPower;
    }
    protected void setPower(int power) {
        mPower = power;
    }
    protected OcResourceHandle getHandle() {
        return mResourceHandle;
    }

    /**
     *  creates a resource. this method internally calls registerResource
     * @return returns the OcResourceHandle after creating and registering the resource
     */
    protected OcResourceHandle createResource0() {
        // entityhandler for registerResource
        OcPlatform.EntityHandler eh = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                // this is where the main logic of simpleserver is handled as different requests (GET, PUT, POST, OBSERVE, DELETE) are handled
                return entityHandler(ocResourceRequest);
            }
        };

        try {
            mResourceHandle = OcPlatform.registerResource(StringConstants.RESOURCE_URI0, StringConstants.RESOURCE_TYPENAME,
                StringConstants.RESOURCE_INTERFACE, eh,
                EnumSet.of(ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE));
        } catch (OcException e) {
            logMessage(TAG + "RegisterResource error. " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
        logMessage(TAG + "Successfully registered resource");
        return mResourceHandle;
    }

    /**
     *  create another resource. this method internally calls registerResource
     */
    private void createResource1() {
        // entityhandler for registerResource
        OcPlatform.EntityHandler eh = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                // this is where the main logic of simpleserver is handled as different requests (GET, PUT, POST, OBSERVE, DELETE) are handled
            return entityHandler(ocResourceRequest);
            }
        };

        try {
            OcPlatform.registerResource(StringConstants.RESOURCE_URI1, StringConstants.RESOURCE_TYPENAME,
                StringConstants.RESOURCE_INTERFACE, eh,
                EnumSet.of(ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE));
        } catch (OcException e) {
            logMessage(TAG + "RegisterResource1 error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
        logMessage(TAG + "Successfully registered resource1");
    }

    /**
     * post representation . Post can act like put or can create a new resource.
     * Gets value from the representation and updates the internal state
     * @param rep current OcRepresentation of the object
     * @return updated OcRepresentation
     */
    private OcRepresentation post(OcRepresentation rep) {
        //create a resource the first time
        if (true == doOnce) {
            createResource1();
            OcRepresentation representation = rep;
            try {
                representation.setValue(StringConstants.CREATED_URI, StringConstants.RESOURCE_URI1);
            } catch (OcException e) {
                Log.e(TAG, e.getMessage());
            }
            doOnce = false;
            return representation;
        }
        // from second time onwards, put
        put(rep);
        return get();
    }

    /**
     * puts representation . Gets value from the representation and updates the internal state
     * @param rep current OcRepresentation of the object
     */
    private void put(OcRepresentation rep) {
        try {
            mState = rep.getValue(StringConstants.STATE);
            mPower = rep.getValue(StringConstants.POWER);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        logMessage(TAG + "Put State: " +  mState + " Name: " + mName + " Power: " + mPower);
    }

    /**
     *  gets the updated representation. Updates the representation with internal state before sending out
     * @return OcRepresentation after updating the values of the lightRepresentation
     */
    protected OcRepresentation get() {
        try {
            mLightRep.setValue(StringConstants.STATE, mState);
            mLightRep.setValue(StringConstants.POWER, mPower);
            mLightRep.setValue(StringConstants.NAME, mName);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        return mLightRep;
    }

    /**
     * this is the main method which handles different incoming requests appropriately.
     * Init is not supported currently.
     * @param request OcResourceRequest from the client
     * @return EntityHandlerResult depending on whether the request was handled successfully or not
     */
    private EntityHandlerResult entityHandler(OcResourceRequest request) {
        EntityHandlerResult result = EntityHandlerResult.ERROR;
        if (null != request) {
            RequestType requestType = request.getRequestType();
            EnumSet<RequestHandlerFlag> requestFlag = request.getRequestHandlerFlagSet();

            if (requestFlag.contains(RequestHandlerFlag.INIT)) {
                logMessage(TAG + "Init");
            }
            if (requestFlag.contains(RequestHandlerFlag.REQUEST)) {
                try {
                    logMessage(TAG + "Request");
                    OcResourceResponse ocResourceResponse = new OcResourceResponse();
                    ocResourceResponse.setRequestHandle(request.getRequestHandle());
                    ocResourceResponse.setResourceHandle(request.getResourceHandle());

                    switch (requestType) {
                        // handle GET request
                        case GET:
                            logMessage("GET");
                            ocResourceResponse.setResponseResult(EntityHandlerResult.OK);
                            ocResourceResponse.setErrorCode(StringConstants.ERROR_CODE);
                            ocResourceResponse.setResourceRepresentation(get());
                            OcPlatform.sendResponse(ocResourceResponse);
                            break;
                        // handle PUT request
                        case PUT:
                            OcRepresentation rep = request.getResourceRepresentation();
                            put(rep);
                            ocResourceResponse.setErrorCode(StringConstants.ERROR_CODE);
                            ocResourceResponse.setResponseResult(EntityHandlerResult.OK);
                            ocResourceResponse.setResourceRepresentation(get());
                            OcPlatform.sendResponse(ocResourceResponse);
                            break;
                        // handle POST request
                        case POST:
                            rep = request.getResourceRepresentation();
                            OcRepresentation rep_post = post(rep);
                            ocResourceResponse.setResourceRepresentation(rep_post);
                            ocResourceResponse.setErrorCode(StringConstants.ERROR_CODE);
                            if (rep_post.hasAttribute(StringConstants.CREATED_URI)) {
                                String createdUri = rep_post.getValue(StringConstants.CREATED_URI);
                                if (createdUri.equals(StringConstants.RESOURCE_URI1)) {
                                    ocResourceResponse.setNewResourceUri(createdUri);
                                    ocResourceResponse.setResponseResult
                                            (EntityHandlerResult.RESOURCE_CREATED);
                                } else {
                                    ocResourceResponse.setResponseResult(EntityHandlerResult.OK);
                                }
                            }
                            OcPlatform.sendResponse(ocResourceResponse);
                            break;
                        // handle DELETE request
                        case DELETE:
                            logMessage(TAG + "DELETE");
                            OcPlatform.unregisterResource(getHandle());
                            OcPlatform.unregisterResource(getHandle());
                            break;

                    }

                    result = EntityHandlerResult.OK;
                } catch(OcException e) {
                    logMessage(TAG + "Error in Request " + e.getMessage());
                    Log.e(TAG, e.getMessage());
                }
            }
            // handle OBSERVER request
            if (requestFlag.contains(RequestHandlerFlag.OBSERVER)) {
                logMessage(TAG + "OBSERVER");
                ObservationInfo observationInfo = request.getObservationInfo();

                switch (observationInfo.getObserveAction()) {
                    case REGISTER:
                        synchronized (mObservationIds) {
                            mObservationIds.add(observationInfo.getOcObservationId());
                        }
                        break;
                    case UNREGISTER:
                        synchronized (mObservationIds) {
                            mObservationIds.remove(observationInfo.getOcObservationId());
                        }
                        break;
                }
                if (null == lightRepThread) {
                    lightRepThread = new LightRepThread(this, mObservationIds);
                    lightRepThread.run();
                }
                result = EntityHandlerResult.OK;
            }
        }
        return result;
    }

    @Override
    public void logMessage(String msg) {
        logMsg(msg);
    }

    public void logMsg(final String text) {
        Intent intent = new Intent("org.iotivity.base.examples.simpleserver");
        intent.putExtra(StringConstants.MESSAGE, text);
        mContext.sendBroadcast(intent);
    }
}
