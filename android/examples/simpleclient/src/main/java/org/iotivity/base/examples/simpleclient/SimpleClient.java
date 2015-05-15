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

package org.iotivity.base.examples.simpleclient;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.util.HashMap;
import java.util.List;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * SimpleClient
 * <p/>
 * SimpleClient is a sample client app which should be started after the simpleServer is started.
 * It finds resources advertised by the server and calls different operations on it (GET, PUT,
 * POST and OBSERVE).
 * This implements IMessageLogger to display messages on the screen
 */
public class SimpleClient extends Activity implements OcPlatform.OnResourceFoundListener,
        IMessageLogger {
    private static final String TAG = "SimpleClient: ";

    private Light myLight;
    private OcResource curResource;

    //for display
    private TextView mEventsTextView;
    private static boolean printOnce = true;

    /**
     * configure OIC platform and call findResource
     */
    private void initOICStack() {
        //create platform config
        PlatformConfig cfg = new PlatformConfig(
                this,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        try {
            /**
             * find all resources
             */
            OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY + "?rt=" + "core.light",
                    OcConnectivityType.WIFI, this);
        } catch (OcException e) {
            logMessage(TAG + "findResource error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
    /**
     *  callback when a resource is found. This method calls doGetLightRepresentation to get the
     *  current values of myLight
     */
    synchronized public void onResourceFound(OcResource ocResource) {
        /**
         *  this may potentially be called by multiple threads at the same time
         */
        synchronized (this) {
            String resourceUri;
            String hostAddress;
            resourceUri = ocResource.getUri();
            hostAddress = ocResource.getHost();
            logMessage(TAG + "Discovered Resource\nUri: " + resourceUri + " \n Host: " + hostAddress);
            // get the resource types
            if (resourceUri.equals(StringConstants.RESOURCE_URI0)) {
                curResource = ocResource;
                doGetLightRepresentation();
            }
        }
    }

    /**
     * get myLight values after observe
     */
    private void doObserveLightRepresentation() {
        // eventhandler for observe()
        OcResource.OnObserveListener onObserveListener = new OcResource.OnObserveListener() {
            @Override
            public void onObserveCompleted(List<OcHeaderOption> ocHeaderOptions,
                                           OcRepresentation ocRepresentation, int seqNum) {
                if (printOnce) {
                    logMessage(TAG + "OBSERVE request was successful");
                    printOnce = false;
                }
                try {
                    boolean state = ocRepresentation.getValue(StringConstants.STATE);
                    int power = ocRepresentation.getValue(StringConstants.POWER);
                    String name = ocRepresentation.getValue(StringConstants.NAME);
                    myLight.setState(state);
                    myLight.setPower(power);
                    myLight.setName(name);
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                logMessage(TAG + "onObserve: Power: " + myLight.getPower());
                if (seqNum > 20) {
                    try {
                        curResource.cancelObserve();
                        logMessage(TAG + "Successfully cancelled observe");
                   } catch (OcException e) {
                        logMessage(TAG + "cancelObserve error. " + e.getMessage());
                        Log.e(TAG, e.getMessage());
                    }
                }

            }

            @Override
            public void onObserveFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };
        try {
            curResource.observe(ObserveType.OBSERVE, new HashMap<String, String>(), onObserveListener);
        } catch (OcException e) {
            logMessage(TAG + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * get the current value of myLight after POST and call doObserveLightRepresentation
     *
     * @param ocRepresentation needed to invoke post()
     */
    private void doOnPost2(OcRepresentation ocRepresentation) {
        // eventhandler for post()
        OcResource.OnPostListener onPostListener2 = new OcResource.OnPostListener() {
            @Override
            public void onPostCompleted(List<OcHeaderOption> ocHeaderOptions, OcRepresentation rep) {
                logMessage(TAG + "POST request was successful");
                String createdUri = rep.getUri();
                if (createdUri.equals(StringConstants.RESOURCE_URI1)) {
                    logMessage(TAG + "Uri of the created resource: " + createdUri);
                } else {
                    try {
                        boolean state = rep.getValue(StringConstants.STATE);
                        int power = rep.getValue(StringConstants.POWER);
                        String name = rep.getValue(StringConstants.NAME);
                        myLight.setState(state);
                        myLight.setPower(power);
                        myLight.setName(name);
                    } catch (OcException e) {
                        Log.e(TAG, e.getMessage());
                    }
                    logMessage(TAG + "onPost\nState: " + myLight.getState() + "\nPower: " +
                            myLight.getPower() + "\nName: " + myLight.getName());
                }
                doObserveLightRepresentation();
            }

            @Override
            public void onPostFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };
        try {
            curResource.post(ocRepresentation, new HashMap<String, String>(), onPostListener2);
        } catch (OcException e) {
            logMessage(TAG + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * create a new resource and update its value.
     */
    private void doPostLightRepresentation() {
        // eventhandler for post()
        OcResource.OnPostListener onPostListener = new OcResource.OnPostListener() {
            @Override
            public void onPostCompleted(List<OcHeaderOption> ocHeaderOptions,
                                        OcRepresentation ocRepresentation) {
                String createdUri = "";
                try {
                    createdUri = ocRepresentation.getValue(StringConstants.CREATED_URI);
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                if (createdUri.equals(StringConstants.RESOURCE_URI1)) {
                    logMessage(TAG + "Uri of the created resource: " + createdUri);
                } else {
                    boolean state = false;
                    try {
                        state = ocRepresentation.getValue(StringConstants.STATE);
                        int power = ocRepresentation.getValue(StringConstants.POWER);
                        String name = ocRepresentation.getValue(StringConstants.NAME);
                        myLight.setState(state);
                        myLight.setPower(power);
                        myLight.setName(name);
                    } catch (OcException e) {
                        Log.e(TAG, e.getMessage());
                    }
                }
                OcRepresentation rep = new OcRepresentation();
                myLight.setState(true);
                myLight.setPower(55);
                try {
                    rep.setValue(StringConstants.POWER, myLight.getPower());
                    rep.setValue(StringConstants.STATE, myLight.getState());
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                doOnPost2(rep);
            }

            @Override
            public void onPostFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };

        OcRepresentation rep = new OcRepresentation();
        myLight.setState(false);
        myLight.setPower(105);
        try {
            rep.setValue(StringConstants.STATE, myLight.getState());
            rep.setValue(StringConstants.POWER, myLight.getPower());
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        try {
            curResource.post(rep, new HashMap<String, String>(), onPostListener);
        } catch (OcException e) {
            logMessage(TAG + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * modify the current value of myLight and call doPostLightRepresentation
     */
    private void doPutLightRepresentation() {
        // eventhandler for put()
        OcResource.OnPutListener onPutListener = new OcResource.OnPutListener() {
            @Override
            public void onPutCompleted(List<OcHeaderOption> ocHeaderOptions,
                                       OcRepresentation ocRepresentation) {
                logMessage(TAG + "PUT resource was successful");
                try {
                    boolean state = ocRepresentation.getValue(StringConstants.STATE);
                    int power = ocRepresentation.getValue(StringConstants.POWER);
                    String name = ocRepresentation.getValue(StringConstants.NAME);
                    myLight.setState(state);
                    myLight.setPower(power);
                    myLight.setName(name);
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }

                logMessage(TAG + "onPutCompleted:\nState:" + myLight.getState() + "\nPower: " +
                        myLight.getPower() + "\nName: " + myLight.getName());
                doPostLightRepresentation();
            }

            @Override
            public void onPutFailed(Throwable throwable) {

                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };

        OcRepresentation rep = new OcRepresentation();
        Log.d(TAG, "myLight settings: power = 15");
        myLight.setState(true);
        myLight.setPower(15);
        try {
            rep.setValue(StringConstants.STATE, myLight.getState());
            rep.setValue(StringConstants.POWER, myLight.getPower());
            rep.setValue(StringConstants.NAME, myLight.getName());
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        try {
            Log.d(TAG, "before calling put");
            curResource.put(rep, new HashMap<String, String>(), onPutListener);
        } catch (OcException e) {
            logMessage(TAG + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "end of put call");
    }

    /**
     * get the existing value of myLight and call doPutLightRepresentation() to modify the current values
     */
    private void doGetLightRepresentation() {
        // eventhandler for get()
        OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> headerOptionList,
                                       OcRepresentation ocRepresentation) {
                logMessage(TAG + "GET resource was successful " + StringConstants.STATE);
                try {
                    boolean state = ocRepresentation.getValue(StringConstants.STATE);
                    int power = ocRepresentation.getValue(StringConstants.POWER);
                    String name = ocRepresentation.getValue(StringConstants.NAME);
                    myLight.setState(state);
                    myLight.setPower(power);
                    myLight.setName(name);
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                logMessage(TAG + "onGetCompleted\nState: " + myLight.getState() + "\nPower: " +
                        myLight.getPower() + "\nName: " + myLight.getName());
                doPutLightRepresentation();
            }

            @Override
            public void onGetFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };

        try {
            curResource.get(new HashMap<String, String>(), onGetListener);
        } catch (OcException e) {
            logMessage(TAG + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * to display on SimpleClient screen
     */
    public class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String message = intent.getStringExtra(StringConstants.MESSAGE);
            logMessage(message);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mEventsTextView = new TextView(this);
        mEventsTextView.setMovementMethod(new ScrollingMovementMethod());
        LinearLayout layout = (LinearLayout) findViewById(R.id.linearLayout);
        layout.addView(mEventsTextView, new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f)
        );
        myLight = new Light();

        initOICStack();
    }

    @Override
    public void logMessage(String text) {
        logMsg(text);
    }

    public void logMsg(final String text) {
        runOnUiThread(new Runnable() {
            public void run() {
                Message msg = new Message();
                msg.obj = text;
                mEventsTextView.append("\n");
                mEventsTextView.append(text);
            }
        });
        Log.i(TAG, text);
        //to print on SimpleServer screen
        Intent intent = new Intent("org.iotivity.base.examples.simpleclient");
        intent.putExtra(StringConstants.MESSAGE, text);
        sendBroadcast(intent);
    }
}
