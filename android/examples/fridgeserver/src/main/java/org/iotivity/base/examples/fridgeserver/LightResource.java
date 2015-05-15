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
 * LightResource
 * <p/>
 * Creates a light resource and performs action based on client requests
 */
public class LightResource extends Resource implements IMessageLogger {
    private Context mContext;

    private static String TAG = "LightResource: ";

    private boolean mIsOn = false;

    /**
     * constructor
     *
     * @param context to enable sending of broadcast messages to be displayed on the user screen
     */
    LightResource(Context context) {
        mContext = context;
        //eventHandler for register lightResource
        OcPlatform.EntityHandler eh = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                // this is where the main logic of LightResource is handled
                return entityHandler(ocResourceRequest);
            }
        };
        try {
            logMessage(TAG + "RegisterLightResource " + StringConstants.LIGHT_URI + " : " +
                    StringConstants.RESOURCE_TYPELIGHT + " : " + StringConstants.RESOURCE_INTERFACE);
            mResourceHandle = OcPlatform.registerResource(StringConstants.LIGHT_URI,
                    StringConstants.RESOURCE_TYPELIGHT, StringConstants.RESOURCE_INTERFACE,
                    eh, EnumSet.of(ResourceProperty.DISCOVERABLE));
        } catch (OcException e) {
            logMessage(TAG + "LightResource registerResource error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * updates the current state of the light (on/ off)
     *
     * @return light is on or off
     */
    private void updateRepresentationValues() {
        try {
            mRepresentation.setValue(StringConstants.ON, mIsOn);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * update the value of mIsOn from the representation
     *
     * @param representation get current state of light
     */
    private void put(OcRepresentation representation) {
        try {
            mIsOn = representation.getValue(StringConstants.ON);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
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
                            response.setErrorCode(StringConstants.OK);
                            updateRepresentationValues();
                            response.setResourceRepresentation(mRepresentation);
                            response.setResponseResult(EntityHandlerResult.OK);
                            OcPlatform.sendResponse(response);
                            result = EntityHandlerResult.OK;
                            break;
                        case PUT:
                            response.setErrorCode(StringConstants.OK);
                            put(request.getResourceRepresentation());
                            updateRepresentationValues();
                            response.setResourceRepresentation(mRepresentation);
                            response.setResponseResult(EntityHandlerResult.OK);
                            OcPlatform.sendResponse(response);
                            result = EntityHandlerResult.OK;
                            break;
                    }
                }
            } catch (OcException e) {
                logMessage(TAG + e.getMessage());
                Log.e(TAG, e.getMessage());
                return EntityHandlerResult.ERROR;
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
