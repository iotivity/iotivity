/*
 * ******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.ToggleButton;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.ca.CaInterface;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * This class is for messaging between the server and the client.
 * It can handle message manually.
 */
public class MessageFragment extends Fragment implements OcResource.OnGetListener,
        OcResource.OnPutListener {

    private static final String TAG          = "OIC_SIMPLE_MESSAGE";
    private final String        EOL          = System.getProperties().getProperty("line.separator");
    private final double        MILLI_PER_SEC = 1000.0;

    private Activity            mActivity;
    private Context             mContext;

    OcPlatform.EntityHandler    mEntityHandler;
    private OcResourceHandle    mResourceHandle;
    private OcResource          mFoundResource;
    private OcConnectivityType  mCurrConnectivity;

    private QualityOfService    mQos         = QualityOfService.LOW;

    private LinearLayout        mServerLayout;
    private LinearLayout        mClientLayout;
    private TextView            mResourceText;
    private TextView            mActionLog;
    private TextView            mResultLog;
    private ToggleButton        mQosToggle;
    private Button              mRegisterButton;
    private Button              mGetButton;
    private Button              mPutButton;
    private Button              mLargeButton;
    private Button              mDiscoverIPButton;
    private Button              mDiscoverBTButton;
    private Button              mDiscoverLEButton;
    private Button              mDiscoverTCPButton;
    private Button              mDiscoverNFCButton;

    private String              mLargeData;
    private boolean             mState;
    private long                mStartTime;
    private long                mEndTime;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mActivity = getActivity();
        mContext = mActivity.getBaseContext();
    }

    View.OnClickListener getButtonListener() {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mFoundResource != null) {
                    mStartTime = System.currentTimeMillis();
                    sendGetToFoundResource(Common.STATE_GET);
                } else {
                    Common.showToast(mContext, "Please discovery first");
                    Log.e(TAG, "get() : resource is null");
                }
            }
        };
    }

    View.OnClickListener putButtonListener() {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mFoundResource != null) {
                    mStartTime = System.currentTimeMillis();
                    sendPutToFoundResource();
                } else {
                    Common.showToast(mContext, "Please discovery first");
                    Log.e(TAG, "put() : resource is null");
                }
            }
        };
    }

    View.OnClickListener getLargeListener() {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mFoundResource != null) {
                    mStartTime = System.currentTimeMillis();
                    sendGetToFoundResource(Common.LARGE_GET);
                } else {
                    Common.showToast(mContext, "Please discovery first");
                    Log.e(TAG, "large() : resource is null");
                }
            }
        };
    }

    View.OnClickListener discoverButtonListener(
            final OcConnectivityType connectivityType) {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i(TAG, "discoverButtonListener");
                initOcPlatform(ModeType.CLIENT);
                mFoundResource = null;
                mCurrConnectivity = connectivityType;

                try {
                    if (OcConnectivityType.CT_ADAPTER_TCP == connectivityType) {
                        OcPlatform.findResource("",
                                OcPlatform.WELL_KNOWN_QUERY,
                                EnumSet.of(OcConnectivityType.CT_ADAPTER_IP),
                                resourceFoundListener, mQos);
                    } else if (OcConnectivityType.CT_ADAPTER_GATT_BTLE == connectivityType) {
                        if (null != Common.getLeAddress())
                        {
                            OcPlatform.findResource("",
                                    Common.getLeAddress() + OcPlatform.WELL_KNOWN_QUERY,
                                    EnumSet.of(OcConnectivityType.CT_ADAPTER_GATT_BTLE),
                                    resourceFoundListener, QualityOfService.LOW);
                        } else {
                            Common.showToast(mContext, "Please scan ble device");
                            Log.e(TAG, "invalid ble device");
                        }
                    } else {
                        OcPlatform.findResource("",
                                OcPlatform.WELL_KNOWN_QUERY,
                                EnumSet.of(connectivityType),
                                resourceFoundListener, mQos);
                    }
                } catch (OcException e) {
                    e.printStackTrace();
                }

                mActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mActionLog.setText("[Action Log]" + EOL);
                        mActionLog.append("Find resource()" + EOL);
                        mActionLog.append("Connectivity : " + connectivityType + EOL);

                        mResultLog.setText("[Result Log]" + EOL);
                        mResultLog.append("Start Time : ");
                        mResultLog.append(Common.getDateCurrentTimeZone() + EOL);
                        mStartTime = System.currentTimeMillis();
                    }
                });
                mServerLayout.setVisibility(View.GONE);
            }
        };
    }

    void sendGetToFoundResource(String command) {
        try {
            Log.i(TAG, "sendGetToFoundResource");

            Map<String, String> queryParameters = new HashMap<String, String>();
            queryParameters.put(Common.GET_COMMAND, command);

            mFoundResource.get(queryParameters, this, mQos);

            mActionLog.setText("[Action Log]" + EOL + "Send get()"+ EOL + "To : ");
            mActionLog.append(mFoundResource.getHost() + mFoundResource.getUri() + EOL);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    void sendPutToFoundResource() {
        try {
            Log.i(TAG, "sendPutToFoundResource");

            OcRepresentation rep = new OcRepresentation();
            rep.setValue(Common.STATE_KEY, !mState);

            Map<String, String> queryParams = new HashMap<>();

            mFoundResource.put(rep, queryParams, this);
            mActionLog.setText("[Action Log]" + EOL);
            mActionLog.append("Send put()" + EOL + "To : ");
            mActionLog.append(mFoundResource.getHost() + mFoundResource.getUri() + EOL);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    OcPlatform.OnResourceFoundListener resourceFoundListener =
            new OcPlatform.OnResourceFoundListener() {
                @Override
                public void onResourceFound(OcResource ocResource) {
                    synchronized (mActivity) {
                        final String resourceUri = ocResource.getUri();
                        Log.i(TAG, "onResourceFound : " + ocResource.getUri());

                        if (resourceUri.contains(Common.RESOURCE_URI)
                                && ocResource.getConnectivityTypeSet().contains(mCurrConnectivity)) {
                            mFoundResource = ocResource;
                            mActivity.runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mEndTime = System.currentTimeMillis();
                                    double flightTime = (double) (mEndTime - mStartTime) / MILLI_PER_SEC;
                                    mResultLog.append("Discovery Time : ");
                                    mResultLog.append(String.format("%.3f", flightTime) + "sec" + EOL);
                                    mActionLog.append(mFoundResource.getHost() + resourceUri + EOL);
                                }
                            });
                        }
                    }
                }

                @Override
                public void onFindResourceFailed(Throwable throwable, String uri) {
                    synchronized (mActivity) {
                        Log.i(TAG, "findResource request has failed");
                        Log.e(TAG, throwable.toString());
                    }
                }
            };

    @Override
    public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        String repUri = ocRepresentation.getUri();
        Log.i(TAG, "onGetCompleted : " + repUri);

        try {
            final String command = ocRepresentation.getValue(Common.GET_COMMAND);

            if (command == null || command.isEmpty()) {
                Log.e(TAG, "Get command is null");
                return;
            } else if (command.equals(Common.STATE_GET)) {
                mState = ocRepresentation.getValue(Common.STATE_KEY);
                mLargeData = "";
            } else if (command.equals(Common.LARGE_GET)) {
                mLargeData = ocRepresentation.getValue(Common.LARGE_KEY);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }

        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mEndTime = System.currentTimeMillis();
                double flightTime = (double) (mEndTime - mStartTime) / MILLI_PER_SEC;

                if (mLargeData == null || mLargeData.isEmpty()) {
                    mResultLog.append(EOL + "Get Light State : " + mState + EOL);
                } else {
                    mResultLog.append(EOL + "Payload Size : " + mLargeData.length() + EOL);
                }

                mResultLog.append("Get Time : " + String.format("%.3f", flightTime) + "sec" + EOL);
            }
        });
    }

    @Override
    public void onGetFailed(Throwable throwable) {
        Log.e(TAG, "Get failed");
    }

    @Override
    public void onPutCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        String repUri = ocRepresentation.getUri();
        Log.i(TAG, "onPutCompleted : " + repUri);

        try {
            mState = ocRepresentation.getValue(Common.STATE_KEY);
        } catch (OcException e) {
            e.printStackTrace();
        }

        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mEndTime = System.currentTimeMillis();
                double flightTime = (double) (mEndTime - mStartTime) / MILLI_PER_SEC;
                mResultLog.append(EOL + "Set Light State : " + !mState + EOL + "Put Time : ");
                mResultLog.append(String.format("%.3f", flightTime) + "sec" + EOL);
            }
        });
    }

    @Override
    public void onPutFailed(Throwable throwable) {
        Log.e(TAG, "Put failed");
    }

    // ******************************************************************************
    // End of the OCF Client specific code
    // ******************************************************************************

    private void createResource() {
        mEntityHandler = new OcPlatform.EntityHandler() {
            @Override
            public EntityHandlerResult handleEntity(
                    OcResourceRequest ocResourceRequest) {
                return mEntityHandler(ocResourceRequest);
            }
        };

        try {
            mResourceHandle = OcPlatform.registerResource(Common.RESOURCE_URI,
                    Common.RESOURCE_TYPE, Common.RESOURCE_INTERFACE,
                    mEntityHandler, Common.RESOURCE_PROPERTIES);

        } catch (OcException e) {
            String errString = "Error : " + e.getErrorCode().toString();
            Log.e(TAG, errString);
            StringWriter sw = new StringWriter();
            e.printStackTrace(new PrintWriter(sw));
            String exceptionAsString = sw.toString();
            Log.e(TAG, exceptionAsString);
            mResourceText.setText(errString);
        }

        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < Common.DATA_SIZE; i++) {
            sb.append('j');
        }
        mLargeData = sb.toString();

        sb.setLength(0);
        sb.append("URI :   " + Common.RESOURCE_URI + EOL);
        sb.append("Type :   " + Common.RESOURCE_TYPE + EOL);
        sb.append("Interface :   " + Common.RESOURCE_INTERFACE + EOL);
        sb.append("Properties :   " + Common.RESOURCE_PROPERTIES.toString() + EOL);
        mResourceText.setText(sb.toString());
        mActionLog.setText("");
        mResultLog.setText("Created resource" + EOL);
    }

    protected EntityHandlerResult mEntityHandler(OcResourceRequest ocResourceRequest) {
        EntityHandlerResult result = EntityHandlerResult.ERROR;
        final StringBuilder sb = new StringBuilder();
        switch (ocResourceRequest.getRequestType()) {
            case GET:
                sb.append("Type :   GET");
                if (sendGetResponse(ocResourceRequest)) {
                    result = EntityHandlerResult.OK;
                }
                break;
            case PUT:
                sb.append("Type :   PUT");
                if (sendPutResponse(ocResourceRequest)) {
                    result = EntityHandlerResult.OK;
                }
                break;
            case DELETE:
                sb.append("Type :   DELETE");
                break;
            case POST:
                sb.append("Type :   POST");
                break;
            default:
                break;
        }
        sb.append(EOL + "Light State :   " + mState);
        sb.append(EOL + "Time :   " + Common.getDateCurrentTimeZone());
        if (result == EntityHandlerResult.ERROR) {
            sb.append(EOL + "!! Error occurred during sending the response !!");
        }

        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mResultLog.setText(sb.toString());
            }
        });

        return result;
    }

    private boolean sendGetResponse(OcResourceRequest ocResourceRequest) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Common.showToast(mContext, "received get command, send response");
            }
        });

        Map<String, String> queryParameters = ocResourceRequest.getQueryParameters();
        final String command = queryParameters.get(Common.GET_COMMAND);

        if (command == null
                || (!command.equals(Common.STATE_GET) && !command.equals(Common.LARGE_GET))) {
            return false;
        }

        OcRepresentation rep = new OcRepresentation();
        try {
            rep.setValue(Common.GET_COMMAND, command);
            if (command.equals(Common.STATE_GET)) {
                rep.setValue(Common.STATE_KEY, mState);
            } else if (command.equals(Common.LARGE_GET)) {
                rep.setValue(Common.LARGE_KEY, mLargeData);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }

        OcResourceResponse response = new OcResourceResponse();

        response.setRequestHandle(ocResourceRequest.getRequestHandle());
        response.setResourceHandle(ocResourceRequest.getResourceHandle());
        response.setResourceRepresentation(rep);
        try {
            OcPlatform.sendResponse(response);
            return true;
        } catch (OcException e) {
            e.printStackTrace();
            return false;
        }
    }

    private boolean sendPutResponse(OcResourceRequest ocResourceRequest) {
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Common.showToast(mContext, "received put command, send response");
            }
        });

        try {
            OcRepresentation rep = ocResourceRequest.getResourceRepresentation();
            if (rep.hasAttribute(Common.STATE_KEY)) {
                mState = rep.getValue(Common.STATE_KEY);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }

        OcResourceResponse response = new OcResourceResponse();
        OcRepresentation rep = new OcRepresentation();

        response.setRequestHandle(ocResourceRequest.getRequestHandle());
        response.setResourceHandle(ocResourceRequest.getResourceHandle());
        response.setResourceRepresentation(rep);
        try {
            OcPlatform.sendResponse(response);
            return true;
        } catch (OcException e) {
            e.printStackTrace();
            return false;
        }
    }

    // ******************************************************************************
    // End of the OCF Server specific code
    // ******************************************************************************

    private void initOcPlatform(ModeType type) {
        // BLE Advertisement is enabled by default.
        CaInterface.setBTConfigure(2);

        PlatformConfig cfg = new PlatformConfig(mActivity, mContext,
                ServiceType.IN_PROC,
                type,
                Common.IP_ADDRESS,
                Common.IP_PORT,
                mQos);
        OcPlatform.Configure(cfg);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_message, container, false);

        mServerLayout = (LinearLayout) rootView.findViewById(R.id.server_layout);
        mClientLayout = (LinearLayout) rootView.findViewById(R.id.client_layout);

        mResourceText = (TextView) rootView.findViewById(R.id.resource_view);
        mActionLog = (TextView) rootView.findViewById(R.id.action_log_view);
        mResultLog = (TextView) rootView.findViewById(R.id.result_log_view);

        mDiscoverIPButton = (Button) rootView.findViewById(R.id.ip_button);
        mDiscoverBTButton = (Button) rootView.findViewById(R.id.bt_button);
        mDiscoverLEButton = (Button) rootView.findViewById(R.id.le_button);
        mDiscoverTCPButton = (Button) rootView.findViewById(R.id.tcp_button);
        mDiscoverNFCButton = (Button) rootView.findViewById(R.id.nfc_button);

        mRegisterButton = (Button) rootView.findViewById(R.id.register_button);
        mGetButton = (Button) rootView.findViewById(R.id.get_button);
        mPutButton = (Button) rootView.findViewById(R.id.put_button);
        mLargeButton = (Button) rootView.findViewById(R.id.large_button);

        mQosToggle = (ToggleButton) rootView.findViewById(R.id.qos_toggle_button);

        mDiscoverIPButton.setOnClickListener(
                discoverButtonListener(OcConnectivityType.CT_ADAPTER_IP));
        mDiscoverBTButton.setOnClickListener(
                discoverButtonListener(OcConnectivityType.CT_ADAPTER_RFCOMM_BTEDR));
        mDiscoverLEButton.setOnClickListener(
                discoverButtonListener(OcConnectivityType.CT_ADAPTER_GATT_BTLE));
        mDiscoverTCPButton.setOnClickListener(
                discoverButtonListener(OcConnectivityType.CT_ADAPTER_TCP));
        mDiscoverNFCButton.setOnClickListener(
                discoverButtonListener(OcConnectivityType.CT_ADAPTER_NFC));

        mRegisterButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mResourceHandle == null) {
                    initOcPlatform(ModeType.SERVER);
                    createResource();
                    mClientLayout.setVisibility(View.GONE);
                } else {
                    Common.showToast(mContext, "Already created resource");
                }
            }
        });
        mGetButton.setOnClickListener(getButtonListener());
        mPutButton.setOnClickListener(putButtonListener());
        mLargeButton.setOnClickListener(getLargeListener());

        mQosToggle
                .setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                    @Override
                    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                        if (b) {
                            mQos = QualityOfService.HIGH;
                        } else {
                            mQos = QualityOfService.LOW;
                        }
                    }
                });

        return rootView;
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
