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

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;
import android.widget.Toast;

/**
 * To execute resource hosting function for android sample application .
 * @author Copyright 2015 Samsung Electronics All Rights Reserved.
 * @see className class :   ResourceHosting</br>
 *
 */

public class ResourceHosting extends Activity implements OnClickListener
{
        private final int OCSTACK_OK = 0;
        private final int OCSTACK_ERROR = 255;
        private final int RESOURCEHOSTING_DO_NOT_THREADRUNNING = -2;

        private String TAG = "ResourceHosting";
        private OcResourceHandle mResourceHandle;
        private String  mIpAddress;
        private TextView mLogTextView;
        private String mLog = "";
        /**
         * To initialize UI Function Setting.
         * @see Class   class : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : onCreate</br>
         */
        protected void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main);
            mLogTextView = (TextView) findViewById(R.id.txtLog);
            findViewById(R.id.btnStartHosting).setOnClickListener(this);
            findViewById(R.id.btnStopHosting).setOnClickListener(this);
            findViewById(R.id.btLogClear).setOnClickListener(this);

            PlatformConfig platformConfigObj;

            platformConfigObj = new PlatformConfig(this,ServiceType.IN_PROC,
                    ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);

            Log.i(TAG, "Before Calling Configure of ocPlatform");
            OcPlatform.Configure(platformConfigObj);
            Log.i(TAG, "Configuration done Successfully");
        }

        /**
         * To execute initOICStack for running Resource hosting.
         * @see Class   class : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : onStart</br>
         */
        @Override
        protected void onStart()
        {
            super.onStart();
            initOICStack();
        }

        /**
         * To terminate Resource hosting.
         * @see Class   class : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : onStop</br>
         */
        @Override
        protected void onStop()
        {
            super.onStop();
            int result;
            result = ResourceHostingTerminate();
            Log.d(TAG, "ResourceHostingTerminate : "+ result);
        }

        protected void onResume()
        {
            super.onResume();
        }

        /**
         * To execute initOICStack for running Resource hosting.
         * @see Class   class : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : onRestart</br>
         */
        @Override
        protected void onRestart()
        {
            super.onRestart();
            initOICStack();
        }

        /**
         * @see Class   class : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : onDestroy</br>
         */
        protected void onDestroy()
        {
            super.onDestroy();
        }

        /**
         * get IpAddress and execute resourceHostingInit() method.
         * @see Class   class : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : initOICStack</br>
         */
        private void initOICStack()
        {
            try
            {
                mIpAddress = getIpAddress();
                int result;
                result = ResourceHostingInit(mIpAddress);
                Log.d(TAG, "ResourceHostingInit : " + result);
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }

        /**
         * @see Class   class :  com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method :  getIpAddress</br>
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

        /**
         * @see Class  class :   com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method method :   onClick</br>
         * @param v view to choice
         */
        public void onClick(View v)
        {
            int getId = v.getId();

            switch (getId)
            {
                case R.id.btnStartHosting:
                    try
                    {
                        int result;
                        result = OICCoordinatorStart();
                        Log.d(TAG, "OICCoordinatorStart : " + result);
                    }
                    catch (Exception e)
                    {
                        Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
                    }
                    break;
                case R.id.btnStopHosting:
                    int result;
                    result = OICCoordinatorStop();
                    Log.d(TAG, "OICCoordinatorStop : "+ result);
                    break;
                case R.id.btLogClear:
                    clearLog();
                default:
                    break;
            }
        }

        /**
         * all clear log view
         * @see Class   class :  com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method :  clearLog</br>
         */
        private void clearLog()
        {
            mLog = "";
            mLogTextView.setText(mLog);
        }

        /**
         * recieve the callback log message.
         * @see Class   class :  com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method :  cbMessage</br>
         * @param msg callback log message
         */
        public void cbMessage(String msg)
        {
            mLog += msg + "\n";
            mLogTextView.setText(mLog);
        }

        /**
         * jni function - OicCorrdinatorstart() method.
         * @see Class   class :    com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method :  OICCoordinatorStart</br>
         * @see Signature signature : ()V</br>
         */
        public native int OICCoordinatorStart();

        /**
         * jni function - OICCoordinatorStop() method.
         * @see Class   class :  com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method :  OICCoordinatorStop</br>
         * @see signature  signature : ()V</br>
         */
        public native int OICCoordinatorStop();

        /**
         * jni function - ResourceHostingInit() method in order to execute OICCoordinatorStart() method.
         * @see Class   class :  com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method :  ResourceHostingInit</br>
         * @param addr ipAddress
         * @see signature signature : (Ljava/lang/String;)V</br>
         */
        public native int ResourceHostingInit(String addr);

        /**
         * jni function - ResourceHostingTerminate() method in order to terminate resource hosting
         * @see Class   class  : com_example_resourcehostingsampleapp_ResourceHosting</br>
         * @see Method  method : ResourceHostingTerminate</br>
         * @see signature signature : ()V</br>
         */
        public native int ResourceHostingTerminate();

    static
    {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("oc_logger");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("NotificationManager");
    }
}
