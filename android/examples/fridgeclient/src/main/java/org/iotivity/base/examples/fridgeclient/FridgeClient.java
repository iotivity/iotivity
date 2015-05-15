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

package org.iotivity.base.examples.fridgeclient;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
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
import java.util.LinkedList;
import java.util.List;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * FridgeClient
 * <p/>
 * FridgeClient is a sample client app which should be started after the fridgeServer is started.
 * It creates DeviceResource, DoorResource, LightResource and performs a get operation on them.
 * This implements IMessageLogger to display messages on the screen
 */
public class FridgeClient extends Activity implements
        OcPlatform.OnResourceFoundListener, IMessageLogger {
    private static String TAG = "FridgeClient: ";

    private MessageReceiver mMessageReceiver = new MessageReceiver();
    private TextView mEventsTextView;
    private String mDeviceName;
    private int mDeviceCode;
    private List<String> ifaces;
    private final List<OcResource> resourceList = new LinkedList<OcResource>();

    /**
     * configure OIC platform and call findResource
     */
    private void initOICStack() {
        PlatformConfig cfg = new PlatformConfig(
                this,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW);

        OcPlatform.Configure(cfg);
        try {
            OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY + "?rt=" + "intel.fridge",
                    OcConnectivityType.WIFI, this);
        } catch (OcException e) {
            logMessage(TAG + " init Error. " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * prints out the appropriate messages depending on the device code
     *
     * @param representation representation of the OcResource
     * @param value          clientDeviceCode
     */
    private void getResponse(OcRepresentation representation, int value) {
        switch (value) {
            case 0:
                // Get on device
                try {
                    logMessage(TAG + "Name of device: " +
                            representation.getValue(StringConstants.DEVICE_NAME));
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                break;
            case 1:
                // get on fridge light
                try {
                    boolean lightOn = representation.getValue(StringConstants.ON);
                    logMessage(TAG + "The fridge light is " +
                            (lightOn ? "" : "not " + "on"));
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                break;
            case 2:
            case 3:
                // get on fridge door(s)
                try {
                    boolean doorOpen = representation.getValue(StringConstants.OPEN);
                    logMessage(TAG + "Door is " + (doorOpen ?
                            "open" : "not open") + " and is on the " +
                            representation.getValue(StringConstants.SIDE) + " side");
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                break;
            case 4:
                // get on fridge random door
                try {
                    logMessage("Name of fridge: " +
                            representation.getValue(StringConstants.DEVICE_NAME));
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                break;
            default:
                logMessage("Unexpected State");
                break;
        }
    }

    /**
     * this method is used to wait for 1 second between calls to different resources.
     * It is added for better readability
     */
    private void doWait() {
        try {
            Thread.sleep(StringConstants.WAIT_TIME);
        } catch (InterruptedException e) {
            logMessage(TAG + "doWait exception: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
    /**
     *  callback when a resource is found. This method calls getResponse with the correct code
     */
    synchronized public void onResourceFound(OcResource ocResource) {
        // eventHandler for onGetListener
        resourceList.add(ocResource);
        OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> headerOptionList, OcRepresentation rep) {
                logMessage(TAG + " Got a response from " + getClientDeviceName());
                getResponse(rep, getClientDeviceCode());
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

        if (ocResource.getUri().equals(StringConstants.RESOURCE_URI)) {
            logMessage(TAG + "Discovered a device with \nHost: " + ocResource.getHost() +
                    ", Uri: " + ocResource.getUri());
        }
        List<String> lightTypes = new LinkedList<>();
        lightTypes.add("intel.fridge.light");
        try {
            OcResource light = OcPlatform.constructResourceObject(ocResource.getHost(),
                    StringConstants.LIGHT, OcConnectivityType.WIFI, false, lightTypes, ifaces);

            List<String> doorTypes = new LinkedList<>();
            doorTypes.add("intel.fridge.door");
            OcResource leftDoor = OcPlatform.constructResourceObject(ocResource.getHost(),
                    StringConstants.LEFT_DOOR, OcConnectivityType.WIFI, false, doorTypes, ifaces);

            OcResource rightDoor = OcPlatform.constructResourceObject(ocResource.getHost(),
                    StringConstants.RIGHT_DOOR, OcConnectivityType.WIFI, false, doorTypes, ifaces);

            OcResource randomDoor = OcPlatform.constructResourceObject(ocResource.getHost(),
                    StringConstants.RANDOM_DOOR, OcConnectivityType.WIFI, false, doorTypes, ifaces);

            List<OcHeaderOption> headerOptions = new LinkedList<>();
            OcHeaderOption apiVersion = new OcHeaderOption(StringConstants.API_VERSION_KEY,
                    StringConstants.API_VERSION);
            OcHeaderOption clientToken = new OcHeaderOption(StringConstants.CLIENT_VERSION_KEY,
                    StringConstants.CLIENT_TOKEN);
            headerOptions.add(apiVersion);
            headerOptions.add(clientToken);
            ocResource.setHeaderOptions(headerOptions);
            /**
             *  wait for 1 second before calling get on different resources.
             *  It is done for better readability.
             *  doWait() is called before each call to get
             */
            doWait();

            setupClientOptions("Device", 0);
            ocResource.get(new HashMap<String, String>(), onGetListener);
            doWait();

            setupClientOptions("Fridge Light", 1);
            light.get(new HashMap<String, String>(), onGetListener);
            doWait();

            setupClientOptions("Left Door", 2);
            leftDoor.get(new HashMap<String, String>(), onGetListener);
            doWait();

            setupClientOptions("Right Door", 3);
            rightDoor.get(new HashMap<String, String>(), onGetListener);
            doWait();

            setupClientOptions("Random Door", 4);
            randomDoor.get(new HashMap<String, String>(), onGetListener);
            doWait();

            resourceList.add(leftDoor);
            leftDoor.deleteResource(new OcResource.OnDeleteListener() {
                @Override
                public void onDeleteCompleted(List<OcHeaderOption> ocHeaderOptions) {
                    logMessage(TAG + "Delete resource successful");
                }

                @Override
                public void onDeleteFailed(Throwable throwable) {
                    if (throwable instanceof OcException) {
                        OcException ocEx = (OcException) throwable;
                        ErrorCode errCode = ocEx.getErrorCode();
                        //do something based on errorCode
                    }
                    Log.e(TAG, throwable.toString());
                }
            });
        } catch (OcException e) {
            logMessage(TAG + "onResourceFound Error. " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fridge_client);
        registerReceiver(mMessageReceiver, new IntentFilter(StringConstants.INTENT));

        mEventsTextView = new TextView(this);
        mEventsTextView.setMovementMethod(new ScrollingMovementMethod());
        LinearLayout layout = (LinearLayout) findViewById(R.id.linearLayout);
        layout.addView(mEventsTextView, new LinearLayout.LayoutParams
                (LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));
        ifaces = new LinkedList<>();
        ifaces.add(StringConstants.RESOURCE_INTERFACE);
        mDeviceCode = -1;
        mDeviceName = "";

        initOICStack();
    }

    public class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String message = intent.getStringExtra(StringConstants.MESSAGE);
            logMessage(message);
        }
    }

    @Override
    public void logMessage(final String text) {
        if (StringConstants.ENABLE_PRINTING) {
            runOnUiThread(new Runnable() {
                public void run() {
                    final Message msg = new Message();
                    msg.obj = text;
                    mEventsTextView.append("\n");
                    mEventsTextView.append(text);
                }
            });
            Log.i(TAG, text);
        }
    }


    private void setupClientOptions(String name, int value) {
        mDeviceName = name;
        mDeviceCode = value;
    }

    private String getClientDeviceName() {
        return mDeviceName;
    }

    private int getClientDeviceCode() {
        return mDeviceCode;
    }


    //method to print the headerOptions received from the server
    void printHeaderOptions(List<OcHeaderOption> headerOptions) {
        for (OcHeaderOption headerOption : headerOptions) {
            if (StringConstants.API_VERSION_KEY == headerOption.getOptionId()) {
                logMessage(TAG + "Server API version in GET response: " +
                        headerOption.getOptionData());
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_fridge_client, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        onStop();
    }

    @Override
    protected void onStop() {
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mMessageReceiver);
        super.onStop();
    }
}
