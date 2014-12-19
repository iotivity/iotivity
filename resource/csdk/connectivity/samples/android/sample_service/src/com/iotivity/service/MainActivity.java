package com.iotivity.service;

import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
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

	private final CharSequence[] mCheckBoxItems = { Network.WIFI.name(),
			Network.LE.name() };

	private final CharSequence[] mDTLSCheckBoxItems = { DTLS.SECURED.name(),
			DTLS.UNSECURED.name() };

	private final CharSequence[] mMsgTyleCheckBoxItems = { MsgType.CON.name(),
			MsgType.NON.name() };

	private enum Mode {
		SERVER, CLIENT, BOTH, UNKNOWN
	};

	private enum Network {
		WIFI, LE
	};

	private enum DTLS {
		SECURED, UNSECURED
	};

	private enum MsgType {
		CON, NON
	};

	private Mode mCurrentMode = Mode.UNKNOWN;

	private ArrayList<Integer> mSelectedItems = new ArrayList<Integer>();

	private ArrayList<Integer> mSRSelectedItems = new ArrayList<Integer>();

	private boolean mCheckedItems[] = { false, false };

	private RelativeLayout mFindResourceLayout = null;

	private RelativeLayout mSendNotificationLayout = null;

	private RelativeLayout mSendRequestLayout = null;
	
	private RelativeLayout mSendRequestSettingLayout = null;

	private RelativeLayout mSendResponseLayout = null;

	private RelativeLayout mReceiveLayout = null;

	private TextView mMode_tv = null;

	private TextView mNetwork_tv = null;

	private EditText mUri_ed = null;

	private EditText mNotification_ed = null;

	private EditText mReqData_ed = null;

	private EditText mRespData_ed = null;

	private Button mFind_btn = null;

	private Button mNotify_btn = null;

	private Button mReqeust_btn = null;
	
	private Button mReqeust_setting_btn = null;

	private Button mResponse_btn = null;

	private Button mRecv_btn = null;

	/**
	 * Defined ConnectivityType in cacommon.c
	 * 
	 * CA_ETHERNET = (1 << 0) CA_WIFI = (1 << 1) CA_EDR = (1 << 2) CA_LE = (1 <<
	 * 3)
	 */
	private int CA_WIFI = (1 << 1);
	private int CA_LE = (1 << 3);
	private int isSecured = 0;
	private int msgType = 0;
	int selectedNetwork = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		// Initialize UI
		mFindResourceLayout = (RelativeLayout) findViewById(R.id.layout_find);
		mSendNotificationLayout = (RelativeLayout) findViewById(R.id.layout_notify);
		mSendRequestLayout = (RelativeLayout) findViewById(R.id.layout_request);
		mSendRequestSettingLayout = (RelativeLayout) findViewById(R.id.layout_request_setting);
		mSendResponseLayout = (RelativeLayout) findViewById(R.id.layout_response);
		mReceiveLayout = (RelativeLayout) findViewById(R.id.layout_receive);

		mMode_tv = (TextView) findViewById(R.id.tv_mode);
		mNetwork_tv = (TextView) findViewById(R.id.tv_network);

		mUri_ed = (EditText) findViewById(R.id.et_uri);
		mNotification_ed = (EditText) findViewById(R.id.et_notification);
		mReqData_ed = (EditText) findViewById(R.id.et_req_data);
		mRespData_ed = (EditText) findViewById(R.id.et_resp_data);

		mFind_btn = (Button) findViewById(R.id.btn_find_resource);
		mNotify_btn = (Button) findViewById(R.id.btn_notify);
		mReqeust_btn = (Button) findViewById(R.id.btn_Request);
		mReqeust_setting_btn  = (Button) findViewById(R.id.btn_Request_setting);
		mResponse_btn = (Button) findViewById(R.id.btn_Response);
		mRecv_btn = (Button) findViewById(R.id.btn_receive);

		mFind_btn.setOnClickListener(mFindResourceHandler);
		mNotify_btn.setOnClickListener(mNotifyHandler);
		mReqeust_btn.setOnClickListener(mSendRequestHandler);
		mReqeust_setting_btn.setOnClickListener(mSendRequestSettingHandler);
		mResponse_btn.setOnClickListener(mSendResponseHandler);
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
		mSendResponseLayout.setVisibility(View.INVISIBLE);
		mReceiveLayout.setVisibility(View.INVISIBLE);

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
			mSendResponseLayout.setVisibility(View.INVISIBLE);
			mReceiveLayout.setVisibility(View.VISIBLE);

			mNetwork_tv.setText("");

		} else if (mCurrentMode == Mode.CLIENT) {

			mFindResourceLayout.setVisibility(View.VISIBLE);
			mSendNotificationLayout.setVisibility(View.INVISIBLE);
			mSendRequestLayout.setVisibility(View.VISIBLE);
			mSendRequestSettingLayout.setVisibility(View.VISIBLE);
			mSendResponseLayout.setVisibility(View.INVISIBLE);
			mReceiveLayout.setVisibility(View.VISIBLE);

			mNetwork_tv.setText("");
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		// Terminate Connectivity Abstraction
		RM.RMTerminate();
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

			if (mCurrentMode == Mode.UNKNOWN || mSelectedItems.size() == 0) {
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

			if (mCurrentMode == Mode.UNKNOWN || mSelectedItems.size() == 0) {
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

			RM.RMFindResource(mUri_ed.getText().toString());

		}
	};

	private OnClickListener mNotifyHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			RM.RMSendNotification(mNotification_ed.getText().toString(), selectedNetwork);

		}
	};

	private OnClickListener mSendRequestHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			RM.RMSendRequest(mReqData_ed.getText().toString(), selectedNetwork, isSecured, msgType);

		}
	};
	
	private OnClickListener mSendRequestSettingHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

			checkMsgSecured("Select DTLS Type");
			checkMsgType("Select Msg Type");
		}
	};

	private OnClickListener mSendResponseHandler = new OnClickListener() {

		@Override
		public void onClick(View v) {

		//RM.RMSendResponse(mRespData_ed.getText().toString(), selectedNetwork);

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
				.setMultiChoiceItems(mCheckBoxItems, mCheckedItems,
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

							} else if (mSelectedItems.get(i) == Network.LE
									.ordinal()) {
								interestedNetwork |= CA_LE;
							}
						}

						RM.RMSelectNetwork(interestedNetwork);
						selectedNetwork = interestedNetwork;

						if (interestedNetwork != 0
								&& mCurrentMode != Mode.UNKNOWN) {
							showModeView();
						}
					}
				}).show();
	}

	private void checkMsgSecured(String title) {

		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
		builder.setTitle(title)
				.setMultiChoiceItems(mDTLSCheckBoxItems, mCheckedItems,
						new DialogInterface.OnMultiChoiceClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which, boolean isChecked) {

								if (isChecked) {

									mSRSelectedItems.add(which);

								} else if (mSRSelectedItems.contains(which)) {

									mSRSelectedItems.remove(Integer
											.valueOf(which));
								}
							}
						})
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {

						for (int i = 0; i < mSRSelectedItems.size(); i++) {

							if (mSRSelectedItems.get(i) == DTLS.SECURED
									.ordinal()) {
								isSecured = 1;

							} else if (mSRSelectedItems.get(i) == DTLS.UNSECURED
									.ordinal()) {
								isSecured = 0;
							}
						}
					}
				}).show();

		mSRSelectedItems.clear();
	}

	private void checkMsgType(String title) {

		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
		builder.setTitle(title)
				.setMultiChoiceItems(mMsgTyleCheckBoxItems, mCheckedItems,
						new DialogInterface.OnMultiChoiceClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which, boolean isChecked) {

								if (isChecked) {

									mSRSelectedItems.add(which);

								} else if (mSRSelectedItems.contains(which)) {

									mSRSelectedItems.remove(Integer
											.valueOf(which));
								}
							}
						})
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {

					@Override
					public void onClick(DialogInterface dialog, int which) {

						for (int i = 0; i < mSRSelectedItems.size(); i++) {

							if (mSRSelectedItems.get(i) == MsgType.CON
									.ordinal()) {
								msgType = 0;

							} else if (mSRSelectedItems.get(i) == MsgType.NON
									.ordinal()) {
								msgType = 1;
							}
						}
						
					}
				}).show();

		mSRSelectedItems.clear();
	}
}
