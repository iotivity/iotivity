/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
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
package org.iotivity.base.examples.DirectPairing;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.text.InputFilter;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ExpandableListView;
import android.widget.ExpandableListView.OnGroupExpandListener;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.Toast;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcDirectPairDevice;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPrmType;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.examples.DirectPairing.Led;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

public class MainActivity extends Activity {
    private static final int BUFFER_SIZE = 1024;
    private static final String TAG = "DirectPairing: ";
    private static int selected = -1;
    private int[] pList;
    private Button save;
    private int prmType;
    private EditText pinText;
    private Led ledResource;
    private ListView mListView;
    private String filePath = "";
    private OcResource led = null;
    private Switch discoverButton;
    private List<String> resourceInterfaces;
    private ArrayAdapter<String> arrayAdapter;
    private List<Object> onGetResponseList;
    private ExpandableListAdapter exlistAdapter;
    OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
        @Override
        public synchronized void onGetCompleted(List<OcHeaderOption> headerOptionList,
                                                OcRepresentation ocRepresentation) {
            onGetResponseList = new ArrayList<Object>();
            ledResource.setState(ocRepresentation.getValueBool("state"));
            ledResource.setPower(ocRepresentation.getValueInt("power"));
            ledResource.setUri(ocRepresentation.getUri());
            runOnUiThread(new Runnable() {
                public void run() {
                    onGetResponseList.add(ledResource);
                    exlistAdapter.notifyDataSetChanged();
                }
            });

            Log.d(TAG, "Got a response from " + ocRepresentation.getUri() +
                    " " + ocRepresentation.getValueBool("state") + " "
                    + ocRepresentation.getValueInt("power") + " " +
                    ocRepresentation.getValueString("name"));
        }

        @Override
        public synchronized void onGetFailed(Throwable throwable) {
            Log.d(TAG, "GET request has failed");
            Log.e(TAG, throwable.toString());
        }
    };
    private ExpandableListView mPairedListDisplay;
    private ArrayAdapter<OcDirectPairDevice> adapter;
    private List<String> pairedList;
    private List<OcDirectPairDevice> discoveredListObj;
    OcPlatform.DirectPairingListener doDPListener = new OcPlatform.DirectPairingListener() {
        @Override
        public void onDirectPairingListener(String devId, int result) {
            Log.d(TAG, "Inside getDPListener");
            if (result == StringConstants.SUCCES_CODE) {
                pairedList.clear();
                pairedList.add(devId);
                onGetResponseList.add(new Led());
                runOnUiThread(new Runnable() {
                    public void run() {
                        mPairedListDisplay.setAdapter(exlistAdapter);
                        int pos = mListView.getCheckedItemPosition();
                        if (pos != ListView.INVALID_POSITION && discoveredListObj.size() != 0) {
                            discoveredListObj.remove(pos);
                            adapter.notifyDataSetChanged();
                        }
                    }
                });
                Log.d(TAG, "direct pair successfull for " + devId);
            } else {
                Log.d(TAG, "direct pairing failed");
                runOnUiThread(new Runnable() {
                    public void run() {
                        Toast.makeText(getApplicationContext(),
                                "Direct Pairing Failed", Toast.LENGTH_SHORT).show();
                    }
                });
            }
            exportCBOR();
        }
    };
    private List<OcDirectPairDevice> discoveredPairableDevices;
    OcPlatform.FindDirectPairingListener finddirectPairing =
            new OcPlatform.FindDirectPairingListener() {
                @Override
                public void onFindDirectPairingListener(List<OcDirectPairDevice> direcPairingDevice) {
                    Log.d(TAG, "find direct pairable successfull ");
                    discoveredListObj.clear();
                    discoveredPairableDevices =
                            new ArrayList<OcDirectPairDevice>(direcPairingDevice);
                    for (int i = 0; i < direcPairingDevice.size(); i++) {
                        OcDirectPairDevice dpPairDevice = direcPairingDevice.get(i);
                        Log.d(TAG, "getHost from Find Direct Pairing " + dpPairDevice.getHost());
                        discoveredListObj.add(dpPairDevice);
                    }

                    runOnUiThread(new Runnable() {
                        public void run() {
                            mListView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
                            mListView.setAdapter(adapter);
                            discoverButton.setChecked(false);
                        }
                    });
                }
            };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        onGetResponseList = new ArrayList<Object>();
        discoveredListObj = new ArrayList<OcDirectPairDevice>();
        pairedList = new ArrayList<String>();

        ledResource = new Led();

        mListView = (ListView) findViewById(R.id.list_view);
        discoverButton = (Switch) findViewById(R.id.discover);
        mPairedListDisplay = (ExpandableListView) findViewById(R.id.list_view_paired_list);


        filePath = getFilesDir().getPath() + "/"; //  data/data/<package>/files/
        //copy CBOR file when application runs first time
        SharedPreferences wmbPreference = PreferenceManager.getDefaultSharedPreferences(this);
        boolean isFirstRun = wmbPreference.getBoolean("FIRSTRUN", true);
        if (isFirstRun) {
            copyCborFromAsset();
            SharedPreferences.Editor editor = wmbPreference.edit();
            editor.putBoolean("FIRSTRUN", false);
            editor.commit();
        }

        initOICStack();

        adapter = new ArrayAdapter<OcDirectPairDevice>(this,
                android.R.layout.simple_list_item_single_choice,
                discoveredListObj);

        arrayAdapter = new ArrayAdapter<String>(
                MainActivity.this,
                android.R.layout.select_dialog_singlechoice);

        save = (Button) findViewById(R.id.saveDiscovered);

        save.setEnabled(false);
        save.setBackgroundColor(Color.GRAY);

        resourceInterfaces = new LinkedList<>();
        resourceInterfaces.add(OcPlatform.DEFAULT_INTERFACE);

        exlistAdapter = new ExpandableListAdapter(pairedList, onGetResponseList,
                MainActivity.this);
        mPairedListDisplay.setAdapter(exlistAdapter);

        // to expand only a single group item.
        mPairedListDisplay.setOnGroupExpandListener(new OnGroupExpandListener() {
            int previousGroup = -1;

            @Override
            public void onGroupExpand(int groupPosition) {
                Log.d(TAG, "Calling GET api on mResource");
                try {
                    if (null != led) led.get(new HashMap<String, String>(), onGetListener);
                } catch (OcException e) {
                    Log.d(TAG, "Error in GET calls");
                    Log.e(TAG, e.getMessage());
                }

                if (groupPosition != previousGroup)
                    mPairedListDisplay.collapseGroup(previousGroup);

                previousGroup = groupPosition;
            }
        });

        save.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {

                AlertDialog.Builder alertPinDialog =
                        new AlertDialog.Builder(MainActivity.this);
                alertPinDialog.setTitle("PIN");
                alertPinDialog.setMessage("Enter Pin");

                pinText = new EditText(MainActivity.this);
                int maxLength = 8;
                InputFilter[] fArray = new InputFilter[1];
                fArray[0] = new InputFilter.LengthFilter(maxLength);
                pinText.setFilters(fArray);
                pinText.setMaxLines(1);
                LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT,
                        LinearLayout.LayoutParams.MATCH_PARENT);
                pinText.setLayoutParams(lp);
                alertPinDialog.setView(pinText);
                alertPinDialog.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        selected = -1;
                        final AlertDialog alertPrmTypeDialog = new AlertDialog.Builder
                                (MainActivity.this)
                                .setSingleChoiceItems(arrayAdapter, -1,
                                        new DialogInterface.OnClickListener() {
                                            @Override
                                            public void onClick(DialogInterface dialog, int which) {
                                                selected = which;
                                                prmType = pList[which];
                                                Log.d(TAG, " prmType " + prmType);
                                            }
                                        })
                                .setTitle("Select Direct Pair Method")
                                .setPositiveButton("OK", null)
                                .setNegativeButton("CANCEL", null)
                                .create();

                        alertPrmTypeDialog.setOnShowListener(
                                new DialogInterface.OnShowListener() {
                                    @Override
                                    public void onShow(DialogInterface dialog) {
                                        Button ok =
                                                alertPrmTypeDialog
                                                        .getButton(AlertDialog.BUTTON_POSITIVE);

                                        Button cancel =
                                                alertPrmTypeDialog
                                                        .getButton(AlertDialog.BUTTON_NEGATIVE);

                                        ok.setOnClickListener(new View.OnClickListener() {
                                            @Override
                                            public void onClick(View view) {
                                                int pos = mListView.getCheckedItemPosition();
                                                if (selected == -1) {
                                                    Toast.makeText(getApplicationContext(),
                                                            "Please Select A Value",
                                                            Toast.LENGTH_SHORT).show();
                                                } else if (pos != ListView.INVALID_POSITION) {
                                                    if (!discoveredListObj.isEmpty()) {
                                                        try {
                                                            OcDirectPairDevice peer =
                                                                    (OcDirectPairDevice)
                                                                            discoveredListObj.
                                                                                    get(pos);
                                                            OcPlatform.doDirectPairing(peer,
                                                                    OcPrmType.convertOcPrmType(prmType),
                                                                    pinText.getText().toString(),
                                                                    doDPListener);
                                                        } catch (OcException e) {
                                                            Log.d(TAG, "do Direct Pairing error: "
                                                                    + e.getMessage());
                                                            Log.e(TAG, e.getMessage());
                                                        }

                                                        alertPrmTypeDialog.dismiss();
                                                    } else {
                                                        alertPrmTypeDialog.dismiss();
                                                    }
                                                }
                                            }
                                        });

                                        cancel.setOnClickListener(new View.OnClickListener() {
                                            @Override
                                            public void onClick(View view) {
                                                alertPrmTypeDialog.dismiss();
                                            }
                                        });
                                    }
                                });

                        alertPrmTypeDialog.show();
                    }
                });
                alertPinDialog.show();
            }
        });

        mListView.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView adapt, View view, int position, long itemId) {
                save.setEnabled(true);
                save.setBackgroundColor(Color.parseColor("#13AEF4"));
                OcDirectPairDevice dpPairDevice =
                        (OcDirectPairDevice) adapt.getItemAtPosition(position);
                Log.d(TAG, "DevId On List Item Clicked " + dpPairDevice.getDevId());
                List<Integer> pTypeList = dpPairDevice.getPairingMethodList();
                pList = new int[pTypeList.size()];
        		Iterator<Integer> iterator = pTypeList.iterator();
		        for(int k = 0; k < pList.length; k++){
        		    pList[k]=iterator.next().intValue();
		        }
                arrayAdapter.clear();
                for (int j = 0; j < pList.length; j++) {
                    Log.d(TAG, " List Item Value " + pList[j]);
                    OcPrmType prmTy = OcPrmType.convertOcPrmType(pList[j]);
                    arrayAdapter.add(prmTy.name());
                }
            }
        });

        discoverButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    save.setEnabled(false);
                    save.setBackgroundColor(Color.GRAY);
                    new GetDiscoveredItems().execute();
                }
            }
        });

    }

    /**
     * Copy svr db CBOR dat file from assets folder to app data files dir
     */
    private void copyCborFromAsset() {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        try {
            inputStream = getAssets().open(StringConstants.OIC_CLIENT_CBOR_DB_FILE);
            File file = new File(filePath);
            //check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(filePath +
                    StringConstants.OIC_CLIENT_CBOR_DB_FILE);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            Log.d(TAG, "Null pointer exception " + e.getMessage());
            Log.e(TAG, e.getMessage());
        } catch (FileNotFoundException e) {
            Log.d(TAG, "CBOR svr db file not found " + e.getMessage());
            Log.e(TAG, e.getMessage());
        } catch (IOException e) {
            Log.d(TAG, StringConstants.OIC_CLIENT_CBOR_DB_FILE + " file copy failed");
            Log.e(TAG, e.getMessage());
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
    }

    void exportCBOR() {
        try {
            File sd = Environment.getExternalStorageDirectory();
            File data = Environment.getDataDirectory();
            if (sd.canWrite()) {
                String currentDBPath = "/data/data/" + getPackageName() +
                        "/files/" + StringConstants.OIC_CLIENT_CBOR_DB_FILE;
                File currentDB = new File(currentDBPath);
                File backupDB = new File(sd, StringConstants.OIC_CLIENT_CBOR_DB_FILE);
                if (currentDB.exists()) {
                    FileChannel src = new FileInputStream(currentDB).getChannel();
                    FileChannel dst = new FileOutputStream(backupDB).getChannel();
                    dst.transferFrom(src, 0, src.size());
                    src.close();
                    dst.close();
                }
            }
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * configure OIC platform and call findResource
     */
    private void initOICStack() {
        //create platform config
        PlatformConfig cfg = new PlatformConfig(this, ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW, filePath + StringConstants.OIC_CLIENT_CBOR_DB_FILE);
        OcPlatform.Configure(cfg);
        try {
            /*
             * Initialize DataBase
             */
            String sqlDbPath = getFilesDir().getAbsolutePath().replace("files", "databases") +
                    File.separator;
            File file = new File(sqlDbPath);
            //check files directory exists
            if (!(file.isDirectory())) {
                file.mkdirs();
                Log.d(TAG, "Sql db directory created at " + sqlDbPath);
            }
            Log.d(TAG, "Sql db directory exists at " + sqlDbPath);
            OcProvisioning.provisionInit(sqlDbPath + StringConstants.OIC_SQL_DB_FILE);
        } catch (OcException e) {
            Log.d(TAG, "provisionInit error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    class GetDiscoveredItems extends AsyncTask<Void, Void, List<String>> {

        ProgressDialog pDialog;

        @Override
        protected void onPreExecute() {
            pDialog = new ProgressDialog(MainActivity.this);
            pDialog.setMessage("Getting Discovered Items");
            pDialog.setCancelable(false);
            pDialog.setCanceledOnTouchOutside(false);
            pDialog.show();
        }

        ;

        @Override
        protected List<String> doInBackground(Void... params) {
            try {
                OcPlatform.findDirectPairingDevices(5, finddirectPairing);
            } catch (OcException e) {
                Log.e(TAG, e.getMessage());
            }
            return null;
        }

        @Override
        protected void onPostExecute(List<String> discoveredItems) {
            pDialog.dismiss();
            for (Iterator iterator = discoveredListObj.iterator(); iterator.hasNext(); ) {
                OcDirectPairDevice ledResource = (OcDirectPairDevice) iterator.next();
                List<String> lightTypes = new LinkedList<>();
                lightTypes.add("core.led");
                try {
                    Log.d(TAG, "Constructing Led Resource");
                    led = OcPlatform.constructResourceObject(ledResource.getHost(),
                            "/a/led", ledResource.getConnectivityTypeSet(),
                            false, //isObservable
                            lightTypes,
                            resourceInterfaces);
                    Log.d(TAG, "Constructed Led Resource");
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }

    }
}
