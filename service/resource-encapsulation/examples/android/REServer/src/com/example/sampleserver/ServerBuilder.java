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

package com.example.sampleserver;

import org.iotivity.ResourceEncapsulation.common.RCSException;
import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;
import org.iotivity.ResourceEncapsulation.server.Builder;
import org.iotivity.ResourceEncapsulation.server.RCSResourceObject.*;
import org.iotivity.ResourceEncapsulation.server.RCSGetResponse;
import org.iotivity.ResourceEncapsulation.server.RCSRequest;
import org.iotivity.ResourceEncapsulation.server.RCSResourceObject;
import org.iotivity.ResourceEncapsulation.server.RCSSetResponse;

import android.os.Message;

public class ServerBuilder {

    public static RCSResourceObject           rcsResourceObject;
    private static ServerBuilderDevActivity   serverBuilderDevActivityInstance;
    private static String                     logMessage;
    private static Message                    msg;

    private final GetRequestHandler           getRequestListener;
    private final SetRequestHandler           setRequestListener;
    private final AddAttributeUpdatedListener attributeUpdatedListener;

    // constructor
    public ServerBuilder() {
        serverBuilderDevActivityInstance = ServerBuilderDevActivity
                .getServerBuilderDevActivity();
        getRequestListener = new GetRequestHandler();
        setRequestListener = new SetRequestHandler();
        attributeUpdatedListener = new AddAttributeUpdatedListener();
    }

    // Create Resource
    public void createResource() {

        Builder builderObject = new Builder("/a/TempSensor",
                "core.TemperatureSensor", "oic.if.");
        builderObject.setDiscoverable(true);
        builderObject.setObservable(true);

        RCSResourceAttributes attr = new RCSResourceAttributes();
        try {
            attr.setValueInt("Temperature", 10);
        } catch (RCSException e) {
            e.printStackTrace();
        }
        builderObject.setAttributes(attr);

        try {
            rcsResourceObject = builderObject.build();
            rcsResourceObject
                    .setSetRequestHandlerPolicy(SetRequestHandlerPolicy.ACCEPTANCE);
        } catch (RCSException e) {
            e.printStackTrace();
        }
    }

    // Set Attribute
    public void setAttribute(int value) {

        rcsResourceObject.setAttribute("Temperature", value);
        logMessage = "Attribute set successfully" + "\n";
        logMessage += "Temperature : " + value;

        serverBuilderDevActivityInstance.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 0;
        serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
    }

    // Set Get Request Listener
    public void setGetRequestListener() {
        rcsResourceObject.setGetRequestListener(getRequestListener);
        rcsResourceObject.setGetRequestHandler();
        logMessage = "Get Handler set successfully." + "\n";

        serverBuilderDevActivityInstance.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 0;
        serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
    }

    // Set Set Request Listener
    public void setSetRequestListener() {
        rcsResourceObject.setSetRequestListener(setRequestListener);
        rcsResourceObject.setSetRequestHandler();
        logMessage = "Set Handler set successfully." + "\n";

        serverBuilderDevActivityInstance.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 0;
        serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
    }

    // Set Attribute Updated Listener
    public void setAttributeUpdatedListener() {

        rcsResourceObject.setAttributeUpdatedListener(attributeUpdatedListener);
        rcsResourceObject.addAttributeUpdatedListener("Temperature");
        logMessage = "Attribte updated listener set successfully." + "\n";

        serverBuilderDevActivityInstance.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 0;
        serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
    }

    // Remove Attribute Updated Listener
    public void removeAttributeUpdatedListener() {

        rcsResourceObject.removeAttributeUpdatedListener("Temperature");
        logMessage = "Attribte updated listener removed successfully." + "\n";

        serverBuilderDevActivityInstance.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 0;
        serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
    }

    /**
     * Listener for receiving Get Request and for Sending Response
     */
    private class GetRequestHandler implements IGetRequestHandler {

        @Override
        public RCSGetResponse onGetRequest(RCSRequest request,
                RCSResourceAttributes attributes) {

            logMessage = "Got a Get request from client sending dafault response \n"
                    + "URI : " + request.getResourceUri() + "\n";
            serverBuilderDevActivityInstance.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 0;
            serverBuilderDevActivityInstance.getHandler().sendMessage(msg);

            RCSGetResponse object = RCSGetResponse.defaultAction();

            return object;
        }
    };

    /**
     * Listener for receiving Set Request and for Sending Response
     */
    private class SetRequestHandler implements ISetRequestHandler {

        @Override
        public RCSSetResponse onSetRequest(RCSRequest request,
                RCSResourceAttributes attributes) {

            logMessage = "Got a Set request from client sending dafault response"
                    + "\n" + "URI : " + request.getResourceUri() + "\n";
            serverBuilderDevActivityInstance.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 0;
            serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
            RCSSetResponse object = RCSSetResponse.defaultAction();
            return object;
        }
    };

    /**
     * Listener for receiving Updated Attribute
     */
    private class AddAttributeUpdatedListener implements
            IAttributeUpdatedListener {
        @Override
        public void onAttributeUpdate(String oldValue, String newValue) {

            logMessage = "attributed updated" + "\n";
            logMessage += "oldValue : " + oldValue + "\n";
            logMessage += "newValue : " + newValue + "\n";
            serverBuilderDevActivityInstance.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 0;
            serverBuilderDevActivityInstance.getHandler().sendMessage(msg);
        }
    };
}
