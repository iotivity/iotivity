package com.iotivity.service;

import java.util.ArrayList;

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

import com.iotivity.sample_service.R;

public class MainActivity extends Activity {

	static RMInterface RM = new RMInterface();

	private final static String TAG = "MainActivity";

	private final CharSequence[] mCheckBoxItems = { Network.WIFI.name(),
			Network.EDR.name(), Network.LE.name() };

	private final CharSequence[] mDTLSCheckBoxItems = { DTLS.SECURED.name(),
			DTLS.UNSECURED.name() };

	private final CharSequence[] mMsgTyleCheckBoxItems = { MsgType.CON.name(),
			MsgType.NON.name() };

	private enum Mode {
		SERVER, CLIENT, BOTH, UNKNOWN
	};

	private enum Network {
		WIFI, EDR, LE
	};

	private enum DTLS {
		SECURED, UNSECURED
	};

	private enum MsgType {
		CON, NON
	};

	private Mode mCurrentMode = Mode.UNKNOWN;

	private ArrayList<Integer> mSelectedItems = new ArrayList<Integer>();

	private RelativeLayout mFindResourceLayout = null;

	private RelativeLayout mSendNotificationLayout = null;

	private RelativeLayout mSendRequestLayout = null;

	private RelativeLayout mSendRequestSettingLayout = null;

	private RelativeLayout mSendResponseNotiSettingLayout = null;

	private RelativeLayout mReceiveLayout = null;

	private RelativeLayout mFindTitleLayout = null;

	private RelativeLayout mRequestTitleLayout = null;

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

	private EditText mPayload_ed = null;

	private Button mFind_btn = null;

	private Button mNotify_btn = null;

	private Button mAdvertiseResource_btn = null;

	private Button mReqeust_btn = null;

	private Button mReqeust_setting_btn = null;

	private Button mResponse_Notify_setting_btn = null;

	private Button mResponse_btn = null;

	private Button mRecv_btn = null;

	private Handler mLogHandler = null;

	/**
	 * Defined ConnectivityType in cacommon.c
	 * 
	 * CA_ETHERNET = (1 << 0) CA_WIFI = (1 << 1) CA_EDR = (1 << 2) CA_LE = (1 <<
	 * 3)
	 */
	private int CA_WIFI = (1 << 1);
	private int CA_EDR = (1 << 2);
	private int CA_LE = (1 << 3);
	private int isSecured = 0;
	private int msgType = 0;
	private int selectedItem = 0;
	int selectedNetwork = 0;

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
		mSendRequestSettingLayout = (RelativeLayout) findViewById(R.id.layout_request_setting_for_client);
		mFindResourceLayout = (RelativeLayout) findViewById(R.id.layout_find);
		mFindTitleLayout = (RelativeLayout) findViewById(R.id.layout_find_title);
		mRequestTitleLayout = (RelativeLayout) findViewById(R.id.layout_request_title);
		mHandleTitleLayout = (RelativeLayout) findViewById(R.id.layout_handle_title);
		mPayLoadClientEditLayout = (RelativeLayout) findViewById(R.id.layout_payload_client_ed);

		// server
		mSendNotificationLayout = (RelativeLayout) findViewById(R.id.layout_notify);
		mPayLoadServerEditLayout = (RelativeLayout) findViewById(R.id.layout_payload_server_ed);
		mSendResponseNotiSettingLayout = (RelativeLayout) findViewById(R.id.layout_request_setting_for_server);
		mServerButtonLayout = (RelativeLayout) findViewById(R.id.layout_server_bt);
		mResponseNotificationTitleLayout = (RelativeLayout) findViewById(R.id.layout_Response_Noti_title);
		mAdvertiseTitleLayout = (RelativeLayout) findViewById(R.id.layout_advertise_title);
		mAdvertiseResourceLayout = (RelativeLayout) findViewById(R.id.layout_advertise_resource);

		mMode_tv = (TextView) findViewById(R.id.tv_mode);
		mNetwork_tv = (TextView) findViewById(R.id.tv_network);

		mUri_ed = (EditText) findViewById(R.id.et_uri);
		mNotification_ed = (EditText) findViewById(R.id.et_notification);
		mReqData_ed = (EditText) findViewById(R.id.et_req_data);
		mPayload_ed = (EditText) findViewById(R.id.et_payload_data_for_server);

		mFind_btn = (Button) findViewById(R.id.btn_find_resource);
		mResponse_btn = (Button) findViewById(R.id.btn_sendresponse);
		mNotify_btn = (Button) findViewById(R.id.btn_notify);
		mAdvertiseResource_btn = (Button) findViewById(R.id.btn_advertise);
		mReqeust_btn = (Button) findViewById(R.id.btn_Request);
		mReqeust_setting_btn = (Button) findViewById(R.id.btn_Request_setting_for_client);
		mResponse_Notify_setting_btn = (Button) findViewById(R.id.btn_Request_setting_for_server);
		mRecv_btn = (Button) findViewById(R.id.btn_receive);

		mFind_btn.setOnClickListener(mFindResourceHandler);
		mResponse_btn.setOnClickListener(mSendResponseHandler);
		mNotify_btn.setOnClickListener(mNotifyHandler);
		mAdvertiseResource_btn.setOnClickListener(mAdvertiseResourceHandler);
		mReqeust_btn.setOnClickListener(mSendRequestHandler);
		mReqeust_setting_btn.setOnClickListener(mSendRequestSettingHandler);
		mResponse_Notify_setting_btn
				.setOnClickListener(mSendResponseNotiSettingHandler);
		mRecv_btn.setOnClickListener(mResponseHandler);

		showSelectModeView();

		// Initialize Connectivity Abstraction
		RM.RMInitialize(getApplicationContext());
		// Select default network(WIFI)
		RM.RMSelectNetwork(CA_WIFI);
		// set handler
		RM.RMRegisterHandler();
	}

	private void showSelectModeView() {

		mFindResourceLayout.setVisibility(View.INVISIBLE);
		mSendNotificationLayout.setVisibility(View.INVISIBLE);
		mSendRequestLayout.setVisibility(View.INVISIBLE);
		mSendRequestSettingLayout.setVisibility(View.INVISIBLE);
		mReceiveLayout.setVisibility(View.INVISIBLE);
		mFindTitleLayout.setVisibility(View.INVISIBLE);
		mRequestTitleLayout.setVisibility(View.INVISIBLE);
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
			mSendRequestSettingLayout.setVisibility(View.INVISIBLE);
			mReceiveLayout.setVisibility(View.VISIBLE);

			mFindTitleLayout.setVisibility(View.INVISIBLE);
			mRequestTitleLayout.setVisibility(View.INVISIBLE);
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
			mSendRequestSettingLayout.setVisibility(View.VISIBLE);
			mReceiveLayout.setVisibility(View.VISIBLE);

			mFindTitleLayout.setVisibility(View.VISIBLE);
			mRequestTitleLayout.setVisibility(View.VISIBLE);
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

			if (mSelectedItems.size() == 0) {
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

			if (mSelectedItems.size() == 0) {
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

			showAlertDialog("Select Network");

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
			RM.RMSendResponse(mNotification_ed.getText().toString(),
					mPayload_ed.getText().toString(), selectedNetwork,
					isSecured);
		}
	};

	private OnClickListener mNotifyHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			DLog.v(TAG, "SendNotification click");
			RM.RMSendNotification(mNotification_ed.getText().toString(),
					mPayload_ed.getText().toString(), selectedNetwork,
					isSecured);
		}
	};

	private OnClickListener mAdvertiseResourceHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			DLog.v(TAG, "AdvertiseResource click");
			RM.RMAdvertiseResource(mNotification_ed.getText().toString(),
					selectedNetwork);
		}
	};

	private OnClickListener mSendRequestHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			DLog.v(TAG, "SendRequest click");
			RM.RMSendRequest(mReqData_ed.getText().toString(), mPayload_ed
					.getText().toString(), selectedNetwork, isSecured, msgType);
		}
	};

	private OnClickListener mSendRequestSettingHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			checkMsgSecured("Select DTLS Type");
			checkMsgType("Select Msg Type");
		}
	};

	private OnClickListener mSendResponseNotiSettingHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			checkMsgSecured("Select DTLS Type");
		}
	};

	private OnClickListener mResponseHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			RM.RMHandleRequestResponse();
		}
	};

	private void showAlertDialog(String title) {

		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
		builder.setTitle(title)
				.setMultiChoiceItems(mCheckBoxItems, null,
						new DialogInterface.OnMultiChoiceClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which, boolean isChecked) {

								if (isChecked) {

									mSelectedItems.add(which);

								} else if (mSelectedItems.contains(which)) {

									mSelectedItems.remove(Integer
											.valueOf(which));
								}
							}
						})
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {

						int interestedNetwork = 0;

						for (int i = 0; i < mSelectedItems.size(); i++) {

							if (mSelectedItems.get(i) == Network.WIFI.ordinal()) {
								interestedNetwork |= CA_WIFI;
							} else if (mSelectedItems.get(i) == Network.EDR
									.ordinal()) {
								interestedNetwork |= CA_EDR;
							} else if (mSelectedItems.get(i) == Network.LE
									.ordinal()) {
								interestedNetwork |= CA_LE;
							}
						}

						RM.RMSelectNetwork(interestedNetwork);
						selectedNetwork = interestedNetwork;
					}
				}).show();

		mSelectedItems.clear();
	}

	private void checkMsgSecured(String title) {

		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

		builder.setTitle(title)
				.setSingleChoiceItems(mDTLSCheckBoxItems, -1,
						new DialogInterface.OnClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								selectedItem = which;
							}
						})
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {

						if (selectedItem == DTLS.SECURED.ordinal()) {
							isSecured = 1;
							DLog.v(TAG, "Send secured message");

							mPayLoadClientEditLayout
									.setVisibility(View.INVISIBLE);

							mPayLoadServerEditLayout
									.setVisibility(View.INVISIBLE);

						} else if (selectedItem == DTLS.UNSECURED.ordinal()) {
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
					}

				}).show();
	}

	private void checkMsgType(String title) {

		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
		builder.setTitle(title)
				.setSingleChoiceItems(mMsgTyleCheckBoxItems, -1,
						new DialogInterface.OnClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								selectedItem = which;
							}
						})
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {

						if (selectedItem == MsgType.CON.ordinal()) {
							msgType = 0;
							DLog.v(TAG, "Message Type is CON");

						} else if (selectedItem == MsgType.NON.ordinal()) {
							msgType = 1;
							DLog.v(TAG, "Message Type is NON");
						}

					}
				}).show();
	}

	public void OnResponseReceived(String subject, String receivedData) {
		String callbackData = subject + receivedData;
		DLog.v(TAG, callbackData);

	}
}