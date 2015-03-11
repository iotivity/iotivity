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

package org.iotivity.base.examples.fridgeserver;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.ResourceProperty;

import java.util.EnumSet;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * DoorResource
 * <p/>
 * Creates a door resource and performs action based on client requests
 */
public class DoorResource extends Resource implements IMessageLogger {
    private Context mContext;

    private static String TAG = "DoorResource: ";
    private String mSide = StringConstants.LEFT;
    private boolean mOpen;
    private String resourceURI;

    /**
     * Constructor
     *
     * @param side    left or right side of the door
     * @param context to enable sending of broadcast messages to be displayed on the user screen
     */
    DoorResource(String side, Context context) {
        mContext = context;
        mSide = side;

        resourceURI = StringConstants.DOOR_URI + mSide;

        // eventHandler for register doorResource
        OcPlatform.EntityHandler eh = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                // this is where the main logic of DoorResource is handled
                return entityHandler(ocResourceRequest);
            }
        };
        try {
            logMessage(TAG + "RegisterDoorResource " + resourceURI + " : " +
                    StringConstants.RESOURCE_TYPEDOOR + " : " + StringConstants.RESOURCE_INTERFACE);
            mResourceHandle = OcPlatform.registerResource(resourceURI,
                    StringConstants.RESOURCE_TYPEDOOR, StringConstants.RESOURCE_INTERFACE,
                    eh, EnumSet.of(ResourceProperty.DISCOVERABLE));
        } catch (OcException e) {
            logMessage(TAG + "DoorResource registerResource error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * updates the current value of the door resource
     *
     * @return door representation
     */
    private void updateRepresentationValues() {
        mRepresentation.setValueString(StringConstants.SIDE, mSide);
        mRepresentation.setValueBool(StringConstants.OPEN, mOpen);
        mRepresentation.setValueString(StringConstants.DEVICE_NAME,
                "Intel Powered 2 door, 1 light refrigerator");
    }

    /**
     * update the OPEN value of doorResource (door is open/ closed)
     *
     * @param representation get current state of door
     */
    private void put(OcRepresentation representation) {
        mOpen = representation.getValueBool(StringConstants.OPEN);
        // Note, we won't let the user change the door side!
    }

    /**
     * this is the main method which handles different incoming requests appropriately.
     *
     * @param request OcResourceRequest from the client
     * @return EntityHandlerResult depending on whether the request was handled successfully or not
     */
    private EntityHandlerResult entityHandler(OcResourceRequest request) {
        EntityHandlerResult result = EntityHandlerResult.ERROR;
        if (null != request) {
            try {
                if (request.getRequestHandlerFlagSet().contains(RequestHandlerFlag.REQUEST)) {
                    OcResourceResponse response = new OcResourceResponse();
                    response.setRequestHandle(request.getRequestHandle());
                    response.setResourceHandle(request.getResourceHandle());

                    switch (request.getRequestType()) {
                        case GET:
                            response.setErrorCode(StringConstants.ERROR_CODE);
                            updateRepresentationValues();
                            response.setResourceRepresentation(mRepresentation);
                            OcPlatform.sendResponse(response);
                            break;
                        case PUT:
                            response.setErrorCode(StringConstants.ERROR_CODE);
                            put(request.getResourceRepresentation());
                            updateRepresentationValues();
                            response.setResourceRepresentation(mRepresentation);
                            OcPlatform.sendResponse(response);
                            break;
                        case POST:
                        case DELETE:
                            response.setResponseResult(EntityHandlerResult.ERROR);
                            OcPlatform.sendResponse(response);
                    }
                    result = EntityHandlerResult.OK;
                }
            } catch (OcException e) {
                logMessage(TAG + e.getMessage());
                Log.e(TAG, e.getMessage());
            }
        }
        return result;
    }

    @Override
    public void logMessage(String msg) {
        logMsg(msg);
        if (StringConstants.ENABLE_PRINTING) {
            Log.i(TAG, msg);
        }
    }

    public void logMsg(final String text) {
        Intent intent = new Intent(StringConstants.INTENT);
        intent.putExtra(StringConstants.MESSAGE, text);
        mContext.sendBroadcast(intent);
    }
}
