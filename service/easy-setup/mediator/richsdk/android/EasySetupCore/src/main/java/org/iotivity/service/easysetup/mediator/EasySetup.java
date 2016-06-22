/**
 * ***************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 * 
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

import android.content.Context;
import android.util.Log;

import java.io.IOException;
import java.util.ArrayList;

/**
 * <To be modified>
 * This is facade class, a single point of contact for Application.
 * It contains set of APIs to do easy setup of the enrolling device.
 * ON-BOARDING - This is a step to establish connectivity between the device & Mediator device.
 * PROVISION   - This is a step where the netowork's detail & credentials are given to the
 * enrolling device.
 */
public class EasySetup {

    private static final String TAG = EasySetup.class.getName();

    private static EasySetup sInstance;

    private static Context mContext;

    private ArrayList<RemoteEnrollee> mRemoteEnrolleeList;

    protected RemoteEnrollee mRemoteEnrollee;

    //function to call the native nativeCreateRemoteEnrollee
    private native RemoteEnrollee nativeCreateRemoteEnrollee();

    static {
        // Load Easy Setup JNI interface
        System.loadLibrary("ESMediatorRich");
        System.loadLibrary("easysetup-jni");
    }

    private EasySetup() {
        mRemoteEnrolleeList = new ArrayList<RemoteEnrollee>();
        mContext = null;
    }

    /**
     * Gives a singleton instance of Easy setup and initialize the easy setup
     */
    public synchronized static EasySetup getInstance(Context context) {
        if (sInstance == null) {
            sInstance = new EasySetup();
            mContext = context;
        }
        return sInstance;
    }

    /**
     * API to create a new RemoteEnrollee instance
     */
    public synchronized RemoteEnrollee createRemoteEnrollee()
    {
        // native call
        mRemoteEnrollee = nativeCreateRemoteEnrollee();
        mRemoteEnrolleeList.add(mRemoteEnrollee);
        return mRemoteEnrollee;
    }

    /**
     * Reset the Easy setup
     */
    public void finish() {
        //Call the stop Provisioning
        //for (RemoteEnrollee remoteEnrollee : mRemoteEnrolleeList)
        //    remoteEnrollee.stopProvisioningProcess();
        }
}
