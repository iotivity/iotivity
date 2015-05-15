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
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.ResourceProperty;

import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * DeviceResource
 * <p/>
 * Creates a device resource and performs action based on client requests
 */
public class DeviceResource extends Resource implements IMessageLogger {
    private Context mContext;

    private static String TAG = "DeviceResource: ";

    /**
     * constructor
     *
     * @param context to enable sending of broadcast messages to be displayed on the user screen
     */
    DeviceResource(Context context) {
        mContext = context;

        // eventHandler for register deviceResource
        OcPlatform.EntityHandler eh = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                // this is where the main logic of DeviceResource is handled
                return entityHandler(ocResourceRequest);
            }
        };

        try {
            logMessage(TAG + "RegisterDeviceResource " + StringConstants.DEVICE_URI + " : " +
                    StringConstants.RESOURCE_TYPENAME + " : " + StringConstants.RESOURCE_INTERFACE);
            mResourceHandle = OcPlatform.registerResource(StringConstants.DEVICE_URI,
                    StringConstants.RESOURCE_TYPENAME, StringConstants.RESOURCE_INTERFACE,
                    eh, EnumSet.of(ResourceProperty.DISCOVERABLE));
        } catch (OcException e) {
            logMessage(TAG + "registerResource error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * update current state of device
     *
     * @return device representation
     */
    private void updateRepresentationValues() {
        try {
            mRepresentation.setValue(StringConstants.DEVICE_NAME,
                    "Intel Powered 2 door, 1 light refrigerator");
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * unregister the resource
     */
    private void deleteDeviceResource() {
        try {
            OcPlatform.unregisterResource(mResourceHandle);
            logMessage(TAG + "Unregister DeviceResource successful");
        } catch (OcException e) {
            logMessage(TAG + e.getMessage());
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
            List<OcHeaderOption> headerOptions = request.getHeaderOptions();
            String clientAPIVersion = "";
            String clientToken = "";

            // search for header options map and look for API version and client token
            for (OcHeaderOption headerOption : headerOptions) {
                int optionId = headerOption.getOptionId();
                if (StringConstants.API_VERSION_KEY == optionId) {
                    clientAPIVersion = headerOption.getOptionData();
                    logMessage(TAG + " Client API Version: " + clientAPIVersion);
                } else if (StringConstants.CLIENT_VERSION_KEY == optionId) {
                    clientToken = headerOption.getOptionData();
                    logMessage(TAG + " Client Token: " + clientToken);
                }
            }
            if (clientAPIVersion.equals(StringConstants.API_VERSION) &&
                    clientToken.equals(StringConstants.CLIENT_TOKEN)) {
                List<OcHeaderOption> serverHeaderOptions = new LinkedList<>();
                OcHeaderOption apiVersion = new OcHeaderOption(StringConstants.API_VERSION_KEY,
                        StringConstants.API_VERSION);
                serverHeaderOptions.add(apiVersion);
                try {
                    if (request.getRequestHandlerFlagSet().contains(RequestHandlerFlag.REQUEST)) {
                        OcResourceResponse response = new OcResourceResponse();
                        response.setRequestHandle(request.getRequestHandle());
                        response.setResourceHandle(request.getResourceHandle());
                        response.setHeaderOptions(serverHeaderOptions);

                        switch (request.getRequestType()) {
                            case GET:
                                response.setErrorCode(StringConstants.OK);
                                response.setResponseResult(EntityHandlerResult.OK);
                                updateRepresentationValues();
                                response.setResourceRepresentation(mRepresentation);
                                OcPlatform.sendResponse(response);
                                break;
                            case DELETE:
                                deleteDeviceResource();
                                response.setErrorCode(StringConstants.OK);
                                response.setResponseResult(EntityHandlerResult.OK);
                                break;
                            case POST:
                                response.setResponseResult(EntityHandlerResult.ERROR);
                                OcPlatform.sendResponse(response);
                                break;
                        }
                        result = EntityHandlerResult.OK;
                    }
                } catch (OcException e) {
                    logMessage(TAG + e.getMessage());
                    Log.e(TAG, e.getMessage());
                }
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
        intent.putExtra("message", text);
        mContext.sendBroadcast(intent);
    }
}
