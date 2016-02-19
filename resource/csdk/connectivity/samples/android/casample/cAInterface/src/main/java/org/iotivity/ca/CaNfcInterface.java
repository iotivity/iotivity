/* *****************************************************************
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

package org.iotivity.ca;

import java.util.ArrayList;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.app.PendingIntent;
import android.content.IntentFilter.MalformedMimeTypeException;
import android.net.NetworkInfo;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.nfc.NfcEvent;
import android.os.Parcelable;
import android.util.Log;



public class CaNfcInterface implements NfcAdapter.CreateNdefMessageCallback {
    private final static String MYTAG = CaNfcInterface.class.getSimpleName();
    private static Context mContext;
    private Activity  mActivity;
    private NfcAdapter mAdapter;
    private NdefMessage mMessage;
    private boolean misInvokeBeam;

    private CaNfcInterface(Context context, Activity activity) {
        Log.d(MYTAG, "NFC  registerNfcReceiver");
        mContext = context;
        mActivity = activity;

        mAdapter = NfcAdapter.getDefaultAdapter(mContext);
        if (null == mAdapter)
        {
            Log.e(MYTAG, "Failed to get the Adapter");
            return;
        }
    }

    private native static void caNativeNfcPacketReceived(byte[] receivedData);
    private native static NdefMessage caNativeNfcCreateNdefMessage(byte[] sendData);
    private native static boolean caNativeNfcInvokeBeam();

    @Override
    public NdefMessage createNdefMessage(NfcEvent event) {
        Log.d(MYTAG, "NFC  createNdefMessage");

        // Returns the already created message
        return mMessage;
    }

    public void processSendRquest(byte[] sendData) {

        Log.d(MYTAG, "NFC  processSendRquest IN");

        mMessage = caNativeNfcCreateNdefMessage(sendData);
        misInvokeBeam = caNativeNfcInvokeBeam();

        if (!misInvokeBeam)
        {
            Log.e(MYTAG, "NFC  Beam error");
        }
    }

    public void caNfcInitialize() {
        Log.d(MYTAG, "caNfcInitialize");

        if ((null == mActivity) || (null == mContext) || (null == mAdapter)) {
            Log.e(MYTAG, "caNfcInitialize failed, invalid parameters");
            return;
        }

        PendingIntent pendingIntent = PendingIntent.getActivity(mActivity, 0,
                                                    new Intent(mActivity, mActivity.getClass())
                                                    .addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP
                                                    | Intent.FLAG_ACTIVITY_CLEAR_TOP), 0);

        IntentFilter ndef = new IntentFilter(NfcAdapter.ACTION_NDEF_DISCOVERED);

        try {
            ndef.addDataType("*/*");    /* Handles all MIME based dispatches.
                                           You should specify only the ones that you need. */
        } catch (MalformedMimeTypeException e) {
            throw new RuntimeException("fail", e);
        }

        IntentFilter[] intentFiltersArray = new IntentFilter[] {ndef, };

        mAdapter.enableForegroundDispatch(mActivity, pendingIntent, intentFiltersArray, null);
        Log.d(MYTAG, " enableForegroundDispatch ");

        mContext.registerReceiver(mReceiver, new IntentFilter(NfcAdapter.ACTION_NDEF_DISCOVERED));
        Log.d(MYTAG, "NFC  caNfcInitialize OUT");
    }

    private static BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(MYTAG, "onReceive broadcast intent updated - disable callback");

            if (intent.getAction().equals(NfcAdapter.ACTION_NDEF_DISCOVERED))
            {
                processIntent(intent);
            }
        }

        private void processIntent(Intent intent) {

            Log.d(MYTAG, "processIntent");
            Parcelable[] rawMsgs = intent
                    .getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);
            NdefMessage msg = (NdefMessage) rawMsgs[0];
            Log.d(MYTAG, msg.getRecords()[0].toMimeType().toString());
            caNativeNfcPacketReceived(msg.getRecords()[0].getPayload());
        }
    };
}
