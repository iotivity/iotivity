package org.iotivity.service;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.iotivity.sample_service.R;

public class MainActivity extends Activity {

    static RMInterface RM = new RMInterface();

    private final static String TAG = "MainActivity";

    private final CharSequence[] mNetworkCheckBoxItems = { Network.IPV4.name(),
            Network.IPV6.name(), Network.EDR.name(), Network.LE.name()};

    private final CharSequence[] mDTLSCheckBoxItems = { DTLS.UNSECURED.name(),
            DTLS.SECURED.name() };

    private final CharSequence[] mMsgTyleCheckBoxItems = { MsgType.CON.name(),
            MsgType.NON.name(), MsgType.ACK.name(), MsgType.RESET.name() };

    private final CharSequence[] mResponseResultCheckBoxItems = {
            ResponseResult.CA_SUCCESS.name(), ResponseResult.CA_CREATED.name(),
            ResponseResult.CA_DELETED.name(), ResponseResult.CA_EMPTY.name(),
            ResponseResult.CA_BAD_REQ.name(), ResponseResult.CA_BAD_OPT.name(),
            ResponseResult.CA_NOT_FOUND.name(),
            ResponseResult.CA_INTERNAL_SERVER_ERROR.name(),
            ResponseResult.CA_RETRANSMIT_TIMEOUT.name() };

    private enum Mode {
        SERVER, CLIENT, BOTH, UNKNOWN
    };

    private enum Network {
        IPV4, IPV6, EDR, LE
    };

    private enum DTLS {
        UNSECURED, SECURED
    };

    private enum MsgType {
        CON, NON, ACK, RESET
    };

    private enum ResponseResult {
        CA_SUCCESS, CA_CREATED, CA_DELETED, CA_EMPTY, CA_BAD_REQ, CA_BAD_OPT,
        CA_NOT_FOUND, CA_INTERNAL_SERVER_ERROR, CA_RETRANSMIT_TIMEOUT
    }

    private boolean mCheckedItems[] = {
            false, false, false, false
    };

    private int mSelectedItems[] = { 0, 0, 0, 0 };

    private int mUnSelectedItems[] = { 0, 0, 0, 0 };

    private Mode mCurrentMode = Mode.UNKNOWN;

    private RelativeLayout mFindResourceLayout = null;

    private RelativeLayout mSendNotificationLayout = null;

    private RelativeLayout mSendRequestLayout = null;

    private RelativeLayout mSendRequestToAllLayout = null;

    private RelativeLayout mSendRequestSettingLayout = null;

    private RelativeLayout mSendRequestToAllSettingLayout = null;

    private RelativeLayout mSendResponseNotiSettingLayout = null;

    private RelativeLayout mReceiveLayout = null;

    private RelativeLayout mFindTitleLayout = null;

    private RelativeLayout mRequestTitleLayout = null;

    private RelativeLayout mRequestToAllTitleLayout = null;

    private RelativeLayout mResponseNotificationTitleLayout = null;

    private RelativeLayout mAdvertiseTitleLayout = null;

    private RelativeLayout mHandleTitleLayout = null;

    private RelativeLayout mPayLoadClientEditLayout = null;

    private RelativeLayout mPayLoadServerEditLayout = null;

    private RelativeLayout mAdvertiseResourceLayout = null;

    private RelativeLayout mServerButtonLayout = null;

    private TextView mMode_tv = null;

    private TextView mNetwork_tv = null;

    private EditText mUri_ed = null;

    private EditText mNotification_ed = null;

    private EditText mReqData_ed = null;

    private EditText mReqToAllData_ed = null;

    private EditText mPayload_ed = null;

    private EditText mAdvertise_ed = null;

    private Button mFind_btn = null;

    private Button mNotify_btn = null;

    private Button mAdvertiseResource_btn = null;

    private Button mReqeust_btn = null;

    private Button mReqeust_setting_btn = null;

    private Button mReqeustToAll_btn = null;

    private Button mReqeustToAll_setting_btn = null;

    private Button mResponse_Notify_setting_btn = null;

    private Button mResponse_btn = null;

    private Button mGetNetworkInfo_btn = null;

    private Button mRecv_btn = null;

    private Handler mLogHandler = null;

    /**
     * Defined ConnectivityType in cacommon.c
     *
     * CA_IPV4 = (1 << 0) CA_IPV6 = (1 << 1) CA_EDR = (1 << 2) CA_LE = (1 << 3)
     */
    private int CA_IPV4 = (1 << 0);
    private int CA_IPV6 = (1 << 1);
    private int CA_EDR = (1 << 2);
    private int CA_LE = (1 << 3);
    private int isSecured = 0;
    private int msgType = 1;
    private int responseValue = 0;
    private int selectedNetworkType = -1;
    private int selectedMsgType = 1;
    private int selectedMsgSecured = 0;
    private int selectedResponseValue = 0;
    int selectedNetwork = -1;
    int interestedNetwork = 0;
    int uninterestedNetwork = 0;
    private boolean isSendResponseSetting = false;
    private boolean isSendRequestToAllSetting = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        RM.setResponseListener(this);

        mLogHandler = new Handler();
        TextView logView = (TextView) findViewById(R.id.tv_result);
        DLog.setTextView(mLogHandler, logView);

        // Initialize UI
        // common
        mReceiveLayout = (RelativeLayout) findViewById(R.id.layout_receive);

        // client
        mSendRequestLayout = (RelativeLayout) findViewById(R.id.layout_request);
        mSendRequestToAllLayout = (RelativeLayout) findViewById(R.id.layout_request_to_all);
        mSendRequestSettingLayout = (RelativeLayout)
                findViewById(R.id.layout_request_setting_for_client);
        mSendRequestToAllSettingLayout = (RelativeLayout)
                findViewById(R.id.layout_request_to_all_setting_for_client);
        mFindResourceLayout = (RelativeLayout) findViewById(R.id.layout_find);
        mFindTitleLayout = (RelativeLayout) findViewById(R.id.layout_find_title);
        mRequestTitleLayout = (RelativeLayout) findViewById(R.id.layout_request_title);
        mRequestToAllTitleLayout = (RelativeLayout) findViewById(R.id.layout_request_to_all_title);
        mHandleTitleLayout = (RelativeLayout) findViewById(R.id.layout_handle_title);
        mPayLoadClientEditLayout = (RelativeLayout) findViewById(R.id.layout_payload_client_ed);

        // server
        mSendNotificationLayout = (RelativeLayout) findViewById(R.id.layout_notify);
        mPayLoadServerEditLayout = (RelativeLayout)
                findViewById(R.id.layout_payload_server_ed);
        mSendResponseNotiSettingLayout = (RelativeLayout)
                findViewById(R.id.layout_request_setting_for_server);
        mServerButtonLayout = (RelativeLayout) findViewById(R.id.layout_server_bt);
        mResponseNotificationTitleLayout = (RelativeLayout)
                findViewById(R.id.layout_Response_Noti_title);
        mAdvertiseTitleLayout = (RelativeLayout) findViewById(R.id.layout_advertise_title);
        mAdvertiseResourceLayout = (RelativeLayout) findViewById(R.id.layout_advertise_resource);

        mMode_tv = (TextView) findViewById(R.id.tv_mode);
        mNetwork_tv = (TextView) findViewById(R.id.tv_network);

        mUri_ed = (EditText) findViewById(R.id.et_uri);
        mNotification_ed = (EditText) findViewById(R.id.et_notification);
        mReqData_ed = (EditText) findViewById(R.id.et_req_data);
        mReqToAllData_ed = (EditText) findViewById(R.id.et_req_to_all_data);
        mPayload_ed = (EditText) findViewById(R.id.et_payload_data_for_server);
        mAdvertise_ed = (EditText) findViewById(R.id.et_uri_advertise);

        mFind_btn = (Button) findViewById(R.id.btn_find_resource);
        mResponse_btn = (Button) findViewById(R.id.btn_sendresponse);
        mNotify_btn = (Button) findViewById(R.id.btn_notify);
        mAdvertiseResource_btn = (Button) findViewById(R.id.btn_advertise);
        mReqeust_btn = (Button) findViewById(R.id.btn_Request);
        mReqeust_setting_btn = (Button) findViewById(R.id.btn_Request_setting_for_client);
        mReqeustToAll_btn = (Button) findViewById(R.id.btn_request_to_all);
        mReqeustToAll_setting_btn = (Button)
                findViewById(R.id.btn_request_to_all_setting_for_client);
        mResponse_Notify_setting_btn = (Button) findViewById(R.id.btn_Request_setting_for_server);
        mGetNetworkInfo_btn = (Button) findViewById(R.id.btn_get_network_info);
        mRecv_btn = (Button) findViewById(R.id.btn_receive);

        mFind_btn.setOnClickListener(mFindResourceHandler);
        mResponse_btn.setOnClickListener(mSendResponseHandler);
        mNotify_btn.setOnClickListener(mNotifyHandler);
        mAdvertiseResource_btn.setOnClickListener(mAdvertiseResourceHandler);
        mReqeust_btn.setOnClickListener(mSendRequestHandler);
        mReqeust_setting_btn.setOnClickListener(mSendRequestSettingHandler);
        mReqeustToAll_btn.setOnClickListener(mSendRequestToAllHandler);
        mReqeustToAll_setting_btn.setOnClickListener(mSendRequestToAllSettingHandler);
        mResponse_Notify_setting_btn
                .setOnClickListener(mSendResponseNotiSettingHandler);
        mRecv_btn.setOnClickListener(mResponseHandler);
        mGetNetworkInfo_btn.setOnClickListener(mGetNetworkInfoHandler);

        showSelectModeView();

        // Initialize Connectivity Abstraction
        RM.RMInitialize(getApplicationContext());

        // set handler
        RM.RMRegisterHandler();
    }

    private void showSelectModeView() {

        mFindResourceLayout.setVisibility(View.INVISIBLE);
        mSendNotificationLayout.setVisibility(View.INVISIBLE);
        mSendRequestLayout.setVisibility(View.INVISIBLE);
        mSendRequestToAllLayout.setVisibility(View.INVISIBLE);
        mSendRequestSettingLayout.setVisibility(View.INVISIBLE);
        mSendRequestToAllSettingLayout.setVisibility(View.INVISIBLE);
        mReceiveLayout.setVisibility(View.INVISIBLE);
        mFindTitleLayout.setVisibility(View.INVISIBLE);
        mRequestTitleLayout.setVisibility(View.INVISIBLE);
        mRequestToAllTitleLayout.setVisibility(View.INVISIBLE);
        mHandleTitleLayout.setVisibility(View.INVISIBLE);
        mPayLoadClientEditLayout.setVisibility(View.INVISIBLE);
        mPayLoadServerEditLayout.setVisibility(View.INVISIBLE);
        mServerButtonLayout.setVisibility(View.INVISIBLE);
        mResponseNotificationTitleLayout.setVisibility(View.INVISIBLE);
        mAdvertiseTitleLayout.setVisibility(View.INVISIBLE);
        mAdvertiseResourceLayout.setVisibility(View.INVISIBLE);
        mSendResponseNotiSettingLayout.setVisibility(View.INVISIBLE);

        mMode_tv.setText("Select Mode (Server or Client)");
    }

    private void showNetworkView() {

        mNetwork_tv.setText("Select Network Type");
    }

    private void showModeView() {

        if (mCurrentMode == Mode.SERVER) {

            mFindResourceLayout.setVisibility(View.INVISIBLE);
            mSendNotificationLayout.setVisibility(View.VISIBLE);
            mSendRequestLayout.setVisibility(View.INVISIBLE);
            mSendRequestToAllLayout.setVisibility(View.INVISIBLE);
            mSendRequestSettingLayout.setVisibility(View.INVISIBLE);
            mSendRequestToAllSettingLayout.setVisibility(View.INVISIBLE);
            mReceiveLayout.setVisibility(View.VISIBLE);

            mFindTitleLayout.setVisibility(View.INVISIBLE);
            mRequestTitleLayout.setVisibility(View.INVISIBLE);
            mRequestToAllTitleLayout.setVisibility(View.INVISIBLE);
            mHandleTitleLayout.setVisibility(View.VISIBLE);
            mPayLoadClientEditLayout.setVisibility(View.INVISIBLE);

            mPayLoadServerEditLayout.setVisibility(View.VISIBLE);
            mServerButtonLayout.setVisibility(View.VISIBLE);

            mResponseNotificationTitleLayout.setVisibility(View.VISIBLE);
            mAdvertiseTitleLayout.setVisibility(View.VISIBLE);
            mAdvertiseResourceLayout.setVisibility(View.VISIBLE);

            mSendResponseNotiSettingLayout.setVisibility(View.VISIBLE);

            mNetwork_tv.setText("");

        } else if (mCurrentMode == Mode.CLIENT) {

            mFindResourceLayout.setVisibility(View.VISIBLE);
            mSendNotificationLayout.setVisibility(View.INVISIBLE);
            mSendRequestLayout.setVisibility(View.VISIBLE);
            mSendRequestToAllLayout.setVisibility(View.VISIBLE);
            mSendRequestSettingLayout.setVisibility(View.VISIBLE);
            mSendRequestToAllSettingLayout.setVisibility(View.VISIBLE);
            mReceiveLayout.setVisibility(View.VISIBLE);

            mFindTitleLayout.setVisibility(View.VISIBLE);
            mRequestTitleLayout.setVisibility(View.VISIBLE);
            mRequestToAllTitleLayout.setVisibility(View.VISIBLE);
            mHandleTitleLayout.setVisibility(View.VISIBLE);
            mPayLoadClientEditLayout.setVisibility(View.VISIBLE);

            mPayLoadServerEditLayout.setVisibility(View.INVISIBLE);
            mServerButtonLayout.setVisibility(View.INVISIBLE);

            mResponseNotificationTitleLayout.setVisibility(View.INVISIBLE);
            mAdvertiseTitleLayout.setVisibility(View.INVISIBLE);
            mAdvertiseResourceLayout.setVisibility(View.INVISIBLE);

            mSendResponseNotiSettingLayout.setVisibility(View.INVISIBLE);

            mNetwork_tv.setText("");
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Terminate Connectivity Abstraction
        RM.RMTerminate();
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        menu.add(0, 1, Menu.NONE, "Start Server");
        menu.add(0, 2, Menu.NONE, "Start Client");
        menu.add(0, 3, Menu.NONE, "Select Network");

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {

        case 1:

            RM.RMStartListeningServer();

            if (interestedNetwork == 0) {
                mCurrentMode = Mode.SERVER;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                showNetworkView();

            } else {
                mCurrentMode = Mode.SERVER;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                showModeView();
            }

            break;

        case 2:

            RM.RMStartDiscoveryServer();

            if (interestedNetwork == 0) {
                mCurrentMode = Mode.CLIENT;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                showNetworkView();

            } else {
                mCurrentMode = Mode.CLIENT;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                showModeView();
            }

            break;

        case 3:

            checkInterestedNetwork("Select Network");

            break;
        }

        return super.onOptionsItemSelected(item);
    }

    private OnClickListener mFindResourceHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            DLog.v(TAG, "FindResource click");
            RM.RMFindResource(mUri_ed.getText().toString());

        }
    };

    private OnClickListener mSendResponseHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            DLog.v(TAG, "SendResponse click");
            if ( selectedNetwork != -1) {
                RM.RMSendResponse(selectedNetwork, isSecured, msgType, responseValue);
            }
            else {
                DLog.v(TAG, "Please Select Network Type");
            }
        }
    };

    private OnClickListener mNotifyHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            DLog.v(TAG, "SendNotification click");
            if ( selectedNetwork != -1) {
                RM.RMSendNotification(mNotification_ed.getText().toString(),
                    mPayload_ed.getText().toString(), selectedNetwork,
                    isSecured, msgType, responseValue);
            }
            else {
                DLog.v(TAG, "Please Select Network Type");
            }
        }
    };

    private OnClickListener mAdvertiseResourceHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            DLog.v(TAG, "AdvertiseResource click");
            RM.RMAdvertiseResource(mAdvertise_ed.getText().toString());
        }
    };

    private OnClickListener mSendRequestHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            DLog.v(TAG, "SendRequest click");
            if ( selectedNetwork != -1) {
                RM.RMSendRequest(mReqData_ed.getText().toString(), mPayload_ed
                    .getText().toString(), selectedNetwork, isSecured, msgType);
            }
            else {
                DLog.v(TAG, "Please Select Network Type");
            }
        }
    };

    private OnClickListener mSendRequestSettingHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {
            checkSendNetworkType("Select Send Network Type");
        }
    };

    private OnClickListener mSendRequestToAllHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            DLog.v(TAG, "SendRequestToAll click");
            if ( selectedNetwork != -1) {
                RM.RMSendReqestToAll(mReqToAllData_ed.getText().toString(), selectedNetwork);
            }
            else {
                DLog.v(TAG, "Please Select Network Type");
            }
        }
    };

    private OnClickListener mSendRequestToAllSettingHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {
            isSendRequestToAllSetting = true;
            checkSendNetworkType("Select Send Network Type");
        }
    };

    private OnClickListener mSendResponseNotiSettingHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {
            isSendResponseSetting = true;
            checkSendNetworkType("Select Send Network Type");
        }
    };

    private OnClickListener mGetNetworkInfoHandler = new OnClickListener() {
        @Override
        public void onClick(View v) {

            RM.RMGetNetworkInfomation();
        }
    };

    private OnClickListener mResponseHandler = new OnClickListener() {

        @Override
        public void onClick(View v) {

            RM.RMHandleRequestResponse();
        }
    };

    private void checkInterestedNetwork(String title) {

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle(title)
                .setMultiChoiceItems(mNetworkCheckBoxItems, mCheckedItems,
                        new DialogInterface.OnMultiChoiceClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog,
                                    int which, boolean isChecked) {

                                if (isChecked) {

                                    mSelectedItems[which] = 1;
                                    mUnSelectedItems[which] = 0;

                                } else if (mSelectedItems[which] == 1) {

                                    mSelectedItems[which] = 0;
                                    mUnSelectedItems[which] = 1;
                                }
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        interestedNetwork = 0;

                        for (int i = 0; i < mSelectedItems.length; i++) {
                            if (mSelectedItems[i] == 1) {
                                if(i != 1)
                                    interestedNetwork |= (1 << i);
                                else
                                    checkNotSupportedTransport("Not Supported Transport");
                            }
                        }
                        if(0 != interestedNetwork)
                            RM.RMSelectNetwork(interestedNetwork);

                        uninterestedNetwork = 0;

                        for (int i = 0; i < mUnSelectedItems.length; i++) {
                            if (mUnSelectedItems[i] == 1) {
                                if (i != 1)
                                    uninterestedNetwork |= (1 << i);
                                else
                                    checkNotSupportedTransport("Not Supported Transport");
                                mUnSelectedItems[i] = 0;
                            }
                        }
                        if(0 != uninterestedNetwork)
                            RM.RMUnSelectNetwork(uninterestedNetwork);

                    }
                }).show();
    }

    private void checkNotSupportedTransport(String title) {

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle(title).
        setMessage("Selected Transport Not Supported")
        .setPositiveButton("OK", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

            }
        }).show();
    }

    private void checkMsgSecured(String title) {

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

        builder.setTitle(title)
                .setSingleChoiceItems(mDTLSCheckBoxItems, selectedMsgSecured,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                selectedMsgSecured = which;
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        if (selectedMsgSecured == DTLS.SECURED.ordinal()) {
                            isSecured = 1;
                            DLog.v(TAG, "Send secured message");

                            mPayLoadClientEditLayout
                                    .setVisibility(View.INVISIBLE);

                            mPayLoadServerEditLayout
                                    .setVisibility(View.INVISIBLE);

                        } else if (selectedMsgSecured == DTLS.UNSECURED.ordinal()) {
                            isSecured = 0;
                            DLog.v(TAG, "Send unsecured message");

                            if (mCurrentMode == Mode.SERVER) {
                                mPayLoadServerEditLayout
                                        .setVisibility(View.VISIBLE);
                            } else if (mCurrentMode == Mode.CLIENT) {
                                mPayLoadClientEditLayout
                                        .setVisibility(View.VISIBLE);
                            }
                        }
                        checkMsgType("Select Msg Type");
                    }

                }).show();
    }

    private void checkMsgType(String title) {

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle(title)
                .setSingleChoiceItems(mMsgTyleCheckBoxItems, selectedMsgType,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                selectedMsgType = which;
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        if (selectedMsgType == MsgType.CON.ordinal()) {
                            msgType = 0;
                            DLog.v(TAG, "Message Type is CON");

                        } else if (selectedMsgType == MsgType.NON.ordinal()) {
                            msgType = 1;
                            DLog.v(TAG, "Message Type is NON");
                        } else if (selectedMsgType == MsgType.ACK.ordinal()) {
                            msgType = 2;
                            DLog.v(TAG, "Message Type is ACK");
                        } else if (selectedMsgType == MsgType.RESET.ordinal()) {
                            msgType = 3;
                            DLog.v(TAG, "Message Type is RESET");
                            }

                        if (isSendResponseSetting == true && msgType != 3) {
                            checkResponseResult("Select Value of Response Result");
                            isSendResponseSetting = false;
                        }
                    }
                }).show();
    }

    private void checkResponseResult(String title) {

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle(title)
                .setSingleChoiceItems(mResponseResultCheckBoxItems, selectedResponseValue,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                selectedResponseValue = which;
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        if (selectedResponseValue == ResponseResult.CA_SUCCESS.ordinal()) {
                            responseValue = 200;
                            DLog.v(TAG, "Response Value is CA_SUCCESS");
                        } else if (selectedResponseValue == ResponseResult.CA_CREATED
                                .ordinal()) {
                            responseValue = 201;
                            DLog.v(TAG, "Response Value is CA_CREATED");
                        } else if (selectedResponseValue == ResponseResult.CA_DELETED
                                .ordinal()) {
                            responseValue = 202;
                            DLog.v(TAG, "Response Value is CA_DELETED");
                        } else if (selectedResponseValue == ResponseResult.CA_EMPTY
                                .ordinal()) {
                            responseValue = 0;
                            DLog.v(TAG, "Response Value is CA_EMPTY");
                        } else if (selectedResponseValue == ResponseResult.CA_BAD_REQ
                                .ordinal()) {
                            responseValue = 400;
                            DLog.v(TAG, "Response Value is CA_BAD_REQ");
                        } else if (selectedResponseValue == ResponseResult.CA_BAD_OPT
                                .ordinal()) {
                            responseValue = 402;
                            DLog.v(TAG, "Response Value is CA_BAD_OPT");
                        } else if (selectedResponseValue == ResponseResult.CA_NOT_FOUND
                                .ordinal()) {
                            responseValue = 404;
                            DLog.v(TAG, "Response Value is CA_NOT_FOUND");
                        } else if (selectedResponseValue ==
                                ResponseResult.CA_INTERNAL_SERVER_ERROR
                                .ordinal()) {
                            responseValue = 500;
                            DLog.v(TAG, "Response Value is CA_INTERNAL_SERVER_ERROR");
                        } else if (selectedResponseValue == ResponseResult.CA_RETRANSMIT_TIMEOUT
                                .ordinal()) {
                            responseValue = 504;
                            DLog.v(TAG, "Response Value is CA_RETRANSMIT_TIMEOUT");
                        }
                    }
                }).show();
    }

    private void checkSendNetworkType(String title) {
        selectedNetworkType = -1;
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

        builder.setTitle(title)
                .setSingleChoiceItems(mNetworkCheckBoxItems, -1,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog,
                                    int which) {
                                selectedNetworkType = which;
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        if (selectedNetworkType == Network.IPV4.ordinal()) {
                            selectedNetwork = CA_IPV4;
                            DLog.v(TAG, "Selected Network is CA_IPV4");
                        } else if (selectedNetworkType == Network.EDR.ordinal()) {
                            selectedNetwork = CA_EDR;
                            DLog.v(TAG, "Selected Network is EDR");
                        } else if (selectedNetworkType == Network.LE.ordinal()) {
                            selectedNetwork = CA_LE;
                            DLog.v(TAG, "Selected Network is LE");
                        }
                        else {
                            DLog.v(TAG, "Selected Network is NULL");
                            selectedNetwork = -1;
                        }

                        if (isSendRequestToAllSetting != true) {
                            checkMsgSecured("Select DTLS Type");
                        }
                        isSendRequestToAllSetting = false;
                    }
                }).show();
    }

    public void OnResponseReceived(String subject, String receivedData) {
        String callbackData = subject + receivedData;
        DLog.v(TAG, callbackData);

    }
}
