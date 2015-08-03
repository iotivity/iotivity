//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file MainActivity.java

package org.iotivity.service.ppm;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class MainActivity extends Activity implements
        android.widget.NumberPicker.OnValueChangeListener {
    class S_demo_device {
        public String m_name;
        public String m_power;
        public int    m_bright;
        public int    m_color;
        public String m_uri;
    }

    static S_demo_device               belkinplug;
    static S_demo_device               gearplug;
    static S_demo_device               hueplug;
    static OcResource                  belkinResource;
    static OcResource                  gearResource;
    static OcResource                  hueResource;
    static TextView                    belkintv;
    static TextView                    geartv;
    static TextView                    huetv;
    static ToggleButton                Hue;
    static android.widget.NumberPicker hue_color;
    static Activity                    mActivity;
    final private static String LOG_TAG = "PPMSampleApp : MainActivity";

    PluginManager                      m_pm;

    Button                             belkinstart;
    Button                             belkingetPlugins;
    Button                             belkingetstate;
    Button                             belkinstop;
    Button                             belkinrescan;
    Button                             Belkin;

    Button                             gearstart;
    Button                             geargetPlugins;
    Button                             geargetstate;
    Button                             gearstop;
    Button                             gearrescan;
    Button                             Gear;

    Button                             huestart;
    Button                             huegetPlugins;
    Button                             huegetstate;
    Button                             huestop;
    Button                             huerescan;

    Context                            ctx;

    String                             name;
    String                             key;
    String                             state;
    String                             id;
    Plugin[]                           user_plugin;
    Map<Integer, Integer>              onValueChangefinalVal    = new HashMap<Integer, Integer>();
    Map<Integer, Boolean>              onValueChangeThreadStart = new HashMap<Integer, Boolean>();
    String[]                           np_h                     = new String[11];
    

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        ctx = getApplicationContext();
        m_pm = new PluginManager(ctx);

        setContentView(R.layout.activity_main);
        mActivity = this;
        onValueChangeThreadStart.put(R.id.np_hue_color, false);
        belkinplug = new S_demo_device();
        gearplug = new S_demo_device();
        hueplug = new S_demo_device();

        for (int i = 0; i < np_h.length; i++) {
            np_h[i] = Integer.toString(i);
        }

        hue_color = (android.widget.NumberPicker) findViewById(R.id.np_hue_color);
        hue_color.setMaxValue(10);
        hue_color.setMinValue(0);
        hue_color.setDisplayedValues(np_h);
        hue_color.setFocusable(true);
        hue_color.setFocusableInTouchMode(true);
        hue_color
                .setDescendantFocusability(android.widget.NumberPicker.FOCUS_BLOCK_DESCENDANTS);
        hue_color.setOnValueChangedListener(this);

        PlatformConfig cfg = new PlatformConfig(this, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);

        OcPlatform.Configure(cfg);

        try {
            FoundResource foundResource = new FoundResource();
            OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), foundResource);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Exception : " + e);
        }

        Belkin = (Button) findViewById(R.id.btn_belkin);
        Belkin.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "Belkin button click listener");
                OcRepresentation rep = new OcRepresentation();
                try {
                    if (belkinplug.m_power == null) {
                        Log.i(LOG_TAG, "m_power is null");
                        belkinplug.m_power = "on";
                        rep.setValue("power", "on");
                    }
                    if (belkinplug.m_power.equals("on")) {
                        Toast.makeText(getApplicationContext(), "Off",
                                Toast.LENGTH_SHORT).show();
                        Log.i(LOG_TAG, "belkin wemo off");
                        rep.setValue("power", "off");
                    } else if (belkinplug.m_power.equals("off")) {
                        Toast.makeText(getApplicationContext(), "On",
                                Toast.LENGTH_SHORT).show();
                        Log.i(LOG_TAG, "belkin wemo on");
                        rep.setValue("power", "on");
                    } else {
                        rep.setValue("power", "on");
                    }
                    rep.setValue("name", "belkin");
                    rep.setValue("uri", "/a/wemo");
                    rep.setValue("brightness", 0);
                    rep.setValue("color", 0);
                } catch (OcException e) {
                    Log.e(LOG_TAG, e.getMessage());
                }
                OnPutBelkinplug onPut = new OnPutBelkinplug();
                if (belkinResource != null) {
                    try {
                        belkinResource.put(rep, new HashMap<String, String>(),
                                onPut);
                    } catch (OcException e) {
                        e.printStackTrace();
                    }
                } else {
                    Toast.makeText(getApplicationContext(), "Belkinplug null",
                            Toast.LENGTH_SHORT).show();
                    Log.i(LOG_TAG, "Belkinplug null");
                }
            }
        });

        belkinstart = (Button) findViewById(R.id.Button01);
        belkinstart.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "start button click listener");
                m_pm.startPlugins("ResourceType", "device.smartplug");

                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(LOG_TAG, "run called!!!");
                        FoundResource foundResource = new FoundResource();
                        try {
                            OcPlatform
                                    .findResource(
                                            "", OcPlatform.WELL_KNOWN_QUERY + "?rt=" + "device.smartplug",
                                            EnumSet.of(OcConnectivityType.CT_DEFAULT), foundResource);
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    }
                }, 3000);
            }
        });

        belkinstop = (Button) findViewById(R.id.Button02);
        belkinstop.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "stop button click listener");
                m_pm.stopPlugins("ResourceType", "device.smartplug");
                belkinResource = null;
                try {
                    java.lang.Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        belkingetPlugins = (Button) findViewById(R.id.Button03);
        belkingetPlugins.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "getPlugins button click listener");
                user_plugin = m_pm.getPlugins();
                // key = "name";
                state = "";
                id = "";

                for (int i = 0; i < user_plugin.length; i++) {
                    System.out.println("value name " + user_plugin[i].getName());
                    System.out.println("value id " + user_plugin[i].getID());
                    // name = user_plugin[i].getID();
                }
            }
        });

        belkingetPlugins = (Button) findViewById(R.id.Button04);
        belkingetPlugins.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "getState click listener");
                state = m_pm.getState("wemo");
                if (state == "")
                    state = "null";
                System.out.println("state: " + state);
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        belkinrescan = (Button) findViewById(R.id.Button05);
        belkinrescan.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "Rescan Plugin click listener");
                m_pm.rescanPlugin();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        Gear = (Button) findViewById(R.id.btn_gear);
        Gear.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "Gear button click listener");
                OcRepresentation rep = new OcRepresentation();
                
                try{   
                    rep.setValue("power", "Happy New Year!");
                    rep.setValue("name", "gear");
                    rep.setValue("uri", "/a/galaxy/gear");
                    rep.setValue("brightness", 0);
                    rep.setValue("color", 0);
                } catch (OcException e) {
                    Log.e(LOG_TAG, e.getMessage());
                }
                
                if (gearResource != null) {
                    Toast.makeText(getApplicationContext(),
                            "Send Noti. to Gear", Toast.LENGTH_SHORT).show();
                    Log.i(LOG_TAG, "Send Noti. to Gear");
                    try {
                        OnPutGear onPut = new OnPutGear();
                        gearResource.put(rep, new HashMap<String, String>(),
                                onPut);
                    } catch (OcException e) {
                        e.printStackTrace();
                    }
                } else {
                    Toast.makeText(getApplicationContext(), "Gear is null",
                            Toast.LENGTH_SHORT).show();
                    Log.i(LOG_TAG, "Gear is null");
                }
            }
        });

        gearstart = (Button) findViewById(R.id.Button06);
        gearstart.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "start button click listener");
                m_pm.startPlugins("ResourceType", "device.notify");

                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(LOG_TAG, "run called!!!");
                        FoundResource foundResource = new FoundResource();
                        try {
                            OcPlatform
                            .findResource(
                                    "", OcPlatform.WELL_KNOWN_QUERY + "?rt=" + "device.notify",
                                    EnumSet.of(OcConnectivityType.CT_DEFAULT), foundResource);
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    }
                }, 3000);
            }
        });

        gearstop = (Button) findViewById(R.id.Button07);
        gearstop.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "stop button click listener");
                m_pm.stopPlugins("ResourceType", "device.notify");
                gearResource = null;
                try {
                    java.lang.Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        geargetPlugins = (Button) findViewById(R.id.Button08);
        geargetPlugins.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "getPlugins button click listener");
                user_plugin = m_pm.getPlugins();
                state = "";
                id = "";

                for (int i = 0; i < user_plugin.length; i++) {
                    System.out.println("value name " + user_plugin[i].getName());
                    System.out.println("value id " + user_plugin[i].getID());
                    name = user_plugin[i].getID();
                }
            }
        });

        geargetPlugins = (Button) findViewById(R.id.Button09);
        geargetPlugins.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "getState click listener");
                state = m_pm.getState("gearnoti");
                if (state == "")
                    state = "null";
                System.out.println("state: " + state);
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        gearrescan = (Button) findViewById(R.id.Button10);
        gearrescan.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "Rescan Plugin click listener");
                m_pm.rescanPlugin();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        Hue = (ToggleButton) findViewById(R.id.tbtn_hue_power);
        Hue.setOnClickListener(new ToggleButton.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "Hue button click listener");
                OcRepresentation rep = new OcRepresentation();
                ToggleButton t = (ToggleButton) v;
                if (t.isChecked()) {
                    Toast.makeText(getApplicationContext(), "Hue ON",
                            Toast.LENGTH_SHORT).show();
                    Log.i(LOG_TAG, "Hue ON");
                    if (hueplug.m_bright == 0) {
                        Log.e(LOG_TAG, "hueplug m_bright is 0");
                        hueplug.m_bright = 128;
                    }
                    try {
                        rep.setValue("power", "on");
                        rep.setValue("name", "hue");
                        rep.setValue("uri", "/a/huebulb");
                        rep.setValue("brightness", hueplug.m_bright);
                        rep.setValue("color", hueplug.m_color);
                    } catch (OcException e) {
                        Log.e(LOG_TAG, e.getMessage());
                    }

                    OnPutHuebulb onPut = new OnPutHuebulb();
                    if (hueResource != null) {
                        try {
                            hueResource.put(rep, new HashMap<String, String>(),
                                    onPut);
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    } else {
                        Toast.makeText(getApplicationContext(),
                                "HueResource null", Toast.LENGTH_SHORT).show();
                        Log.i(LOG_TAG, "HueResource null");
                    }
                } else {
                    Toast.makeText(getApplicationContext(), "Hue OFF",
                            Toast.LENGTH_SHORT).show();
                    Log.i(LOG_TAG, "Hue OFF");
                    if (hueplug.m_bright == 0) {
                        Log.e(LOG_TAG, "hueplug m_bright is 0");
                        hueplug.m_bright = 128;
                    }
                    try {
                        rep.setValue("power", "off");
                        rep.setValue("name", "hue");
                        rep.setValue("uri", "/a/huebulb");
                        rep.setValue("brightness", hueplug.m_bright);
                        rep.setValue("color", hueplug.m_color);
                    } catch (OcException e) {
                        Log.e(LOG_TAG, e.getMessage());
                    }

                    OnPutHuebulb onPut = new OnPutHuebulb();

                    if (hueResource != null) {
                        try {
                            hueResource.put(rep, new HashMap<String, String>(),
                                    onPut);
                        } catch (OcException e) {
                            e.printStackTrace();
                            Log.e(LOG_TAG, e.getMessage());
                        }
                    } else {
                        Toast.makeText(getApplicationContext(),
                                "HueResource null", Toast.LENGTH_SHORT).show();
                        Log.i(LOG_TAG, "HueResource null");
                    }
                }
            }
        });

        huestart = (Button) findViewById(R.id.Button11);
        huestart.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "start button click listener");
                m_pm.startPlugins("ResourceType", "device.light");

                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(LOG_TAG, "run called!!!");
                        FoundResource foundResource = new FoundResource();
                        try {
                            OcPlatform
                            .findResource(
                                    "", OcPlatform.WELL_KNOWN_QUERY + "?rt=" + "device.light",
                                    EnumSet.of(OcConnectivityType.CT_DEFAULT), foundResource);
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                    }
                }, 3000);
            }
        });

        huestop = (Button) findViewById(R.id.Button12);
        huestop.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "stop button click listener");
                m_pm.stopPlugins("ResourceType", "device.light");
                hueResource = null;
                try {
                    java.lang.Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        huegetPlugins = (Button) findViewById(R.id.Button13);
        huegetPlugins.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "getPlugins button click listener");
                user_plugin = m_pm.getPlugins();
                key = "name";
                state = "";
                id = "";

                for (int i = 0; i < user_plugin.length; i++) {
                    System.out.println("value name " + user_plugin[i].getName());
                    System.out.println("value id " + user_plugin[i].getID());
                    name = user_plugin[i].getID();
                }
            }
        });

        huegetPlugins = (Button) findViewById(R.id.Button14);
        huegetPlugins.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "getState click listener");
                state = m_pm.getState("hue");
                if (state == "")
                    state = "null";
                System.out.println("state: " + state);
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        huerescan = (Button) findViewById(R.id.Button15);
        huerescan.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                Log.i(LOG_TAG, "Rescan Plugin click listener");
                m_pm.rescanPlugin();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    static public void updateConnectStatus(String device, boolean status) {
        if (device.equals("belkinplug")) {
            if (status) {
                Log.i(LOG_TAG, "belkingplug status green");
                Toast.makeText(mActivity.getApplicationContext(),
                        "Belkin Connected", Toast.LENGTH_SHORT).show();
                Log.i(LOG_TAG, "Belkin Connected");
            } else {
                Log.i(LOG_TAG, "belkingplug status gray");
                Toast.makeText(mActivity.getApplicationContext(),
                        "Belkin Disonnected", Toast.LENGTH_SHORT).show();
                Log.i(LOG_TAG, "Belkin Disonnected");
            }
        } else if (device.equals("gear")) {
            if (status) {
                Log.i(LOG_TAG, "gear status green");
                Toast.makeText(mActivity.getApplicationContext(),
                        "Gear Connected", Toast.LENGTH_SHORT).show();
                Log.i(LOG_TAG, "Gear Connected");
            } else {
                Log.i(LOG_TAG, "gear status gray");
                Toast.makeText(mActivity.getApplicationContext(),
                        "Gear Disconnected", Toast.LENGTH_SHORT).show();
                Log.i(LOG_TAG, "Gear Disonnected");
            }
        } else if (device.equals("huebulb")) {
            if (status) {
                Log.i(LOG_TAG, "huebulb status green");
                Toast.makeText(mActivity.getApplicationContext(),
                        "Hue Connected", Toast.LENGTH_SHORT).show();
                Log.i(LOG_TAG, "Hue Connected");
            } else {
                Log.i(LOG_TAG, "huebulb status gray");
                Toast.makeText(mActivity.getApplicationContext(),
                        "Hue Disconnected", Toast.LENGTH_SHORT).show();
                Log.i(LOG_TAG, "Hue Disconnected");
            }

            if (hueplug.m_power.equals("on")) {
                Hue.setChecked(true);
            } else {
                Hue.setChecked(false);
            }
        }
    }

    static public void updateBelkinStatus() {
        updateConnectStatus("belkinplug", true);
    }

    static public void updateGearStatus() {
        updateConnectStatus("gear", true);
    }

    static public void updateHuebulbStatus() {
        updateConnectStatus("huebulb", true);
    }

    @Override
    protected void onResume() {
        Log.i(LOG_TAG, "onResume()");
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.i(LOG_TAG, "onPause()");
        super.onPause();
        finish();
    }

    @Override
    protected void onStop() {
        Log.i(LOG_TAG, "onStop()");
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        Log.i(LOG_TAG, "onDestroy()");
        System.exit(1);
        super.onDestroy();
    }

    private class ValueChangeThread extends Thread {
        int idx;

        public ValueChangeThread(int idx) {
            this.idx = idx;
        }

        public void run() {
            onValueChangeThreadStart.put(idx, true);
            try {
                sleep(1000, 0);
            } catch (Exception e) {
                Log.i(LOG_TAG, "waitForFinal exception : " + e);
            }
            Log.i(LOG_TAG, "Final Value for NUMBERPICKER[" + idx + "] : "
                    + onValueChangefinalVal.get(idx));

            if (idx == R.id.np_hue_color) {
                hueplug.m_color = 6300 * onValueChangefinalVal.get(idx);
                Log.i(LOG_TAG, "m_color = " + hueplug.m_color);

                OcRepresentation rep = new OcRepresentation();
                if (hueplug.m_power == null) {
                    hueplug.m_power = "on";
                }
                if (hueplug.m_bright == 0) {
                    hueplug.m_bright = 128;
                }
                try{
                    rep.setValue("power", hueplug.m_power);
                    rep.setValue("name", "huebulb");
                    rep.setValue("uri", "/a/huebulb");
                    rep.setValue("brightness", hueplug.m_bright = 180);
                    rep.setValue("color", hueplug.m_color);
                } catch (OcException e) {
                    Log.e(LOG_TAG, e.getMessage());
                }

                OnPutHuebulb onPut = new OnPutHuebulb();
                if (hueResource != null) {
                    Log.i(LOG_TAG, "huebulbResource is not null");
                    try {
                        hueResource.put(rep, new HashMap<String, String>(),
                                onPut);
                    } catch (OcException e) {
                        e.printStackTrace();
                    }
                } else {
                    Log.i(LOG_TAG, "huebulbResource is null");
                }
            }
            onValueChangeThreadStart.put(idx, false);
        }
    }

    public void onValueChange(android.widget.NumberPicker picker, int oldVal,
            int newVal) {
        final int pickerIdx = picker.getId();

        Thread waitForFinal = new ValueChangeThread(pickerIdx);
        if (onValueChangeThreadStart.get(pickerIdx) == false)
            waitForFinal.start();

        onValueChangefinalVal.put(pickerIdx, newVal);
    }
}