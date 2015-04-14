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
package com.example.resourcehostingsampleapp;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import org.iotivity.base.OcResourceHandle;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;
import android.widget.Toast;


public class ResourceHosting extends Activity implements OnClickListener
{


        private String TAG = "ResourceHosting";
        private OcResourceHandle mResourceHandle;
        private String  mIpAddress;
        private TextView mLogTextView;
        private String mLog = "";
        /*
         * To initialize UI Function Setting
         */
        protected void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main);
            mLogTextView = (TextView) findViewById(R.id.txtLog);
            findViewById(R.id.btnStartHosting).setOnClickListener(this);
            findViewById(R.id.btnStopHosting).setOnClickListener(this);
            findViewById(R.id.btLogClear).setOnClickListener(this);

        }
        /*
         * To execute initOICStack for running Resource hosting
         */
        @Override
        protected void onStart()
        {
            super.onStart();
            initOICStack();

        }
        /*
         * To terminate Resource hosting
         */
        @Override
        protected void onStop()
        {
            super.onStop();
            ResourceHostingTerminate();
        }

        protected void onResume()
        {
            super.onResume();

        }
        /*
         * To execute initOICStack for running Resource hosting
         */
        @Override
        protected void onRestart()
        {
            super.onRestart();
            initOICStack();

        }
        protected void onDestroy()
        {
            super.onDestroy();
        }

        private void initOICStack()
        {
            try
            {
                mIpAddress = getIpAddress();
                ResourceHostingInit(mIpAddress);
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        /*
         * To get IpAddress(IPv4)
         */
        private String getIpAddress()
        {
            try
            {
                for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
                     en.hasMoreElements();)
                {
                    NetworkInterface intf = (NetworkInterface) en.nextElement();
                    for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();)
                    {
                        InetAddress inetAddress = (InetAddress) enumIpAddr.nextElement();
                        if (!inetAddress.isLoopbackAddress())
                        {
                            if (inetAddress instanceof Inet4Address)
                                return inetAddress.getHostAddress().toString();
                        }
                    }
                }
            }
            catch (SocketException e)
            {
                e.printStackTrace();
            }
            return null;
        }

        public void onClick(View v)
        {

            int getId = v.getId();

            switch (getId)
            {
                case R.id.btnStartHosting:

                    try
                    {
                        OICCoordinatorStart();
                    }
                    catch (Exception e)
                    {
                        Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
                    }
                    break;

                case R.id.btnStopHosting:

                    OICCoordinatorStop();
                    break;
                case R.id.btLogClear:
                    clearLog();
                default:
                    break;

            }

        }
        private void clearLog()
        {
            mLog = "";
            mLogTextView.setText(mLog);
        }
        public void cbMessage(String msg)
        {
            mLog += msg + "\n";
            mLogTextView.setText(mLog);
        }

        public native void OICCoordinatorStart();

        public native void OICCoordinatorStop();

        public native void ResourceHostingInit(String addr);

        public native void ResourceHostingTerminate();

        static
        {
            System.loadLibrary("oc_logger");
            System.loadLibrary("oc_logger_core");
            System.loadLibrary("coap");
            System.loadLibrary("octbstack");
            System.loadLibrary("oc");
            System.loadLibrary("ocstack-jni");
            System.loadLibrary("NotificationManager");
        }
}
