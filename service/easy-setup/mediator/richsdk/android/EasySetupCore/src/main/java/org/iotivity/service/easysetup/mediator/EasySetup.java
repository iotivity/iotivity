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

import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

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
    public static final String PROV_RESOURCE_TYPE = "ocf.wk.prov";
    private static EasySetup sInstance;

    private static Context mContext;

    private ArrayList<RemoteEnrollee> mRemoteEnrolleeList;

    protected RemoteEnrollee mRemoteEnrollee;

    //function to call the native nativeCreateRemoteEnrollee
    private native RemoteEnrollee nativeCreateRemoteEnrollee(String Host, String Uri, String devID,
                                                             Boolean isObservable, int conType);
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
    public synchronized RemoteEnrollee createRemoteEnrollee(OcResource enrolleeResource)
    {
        // native call
        String Host = enrolleeResource.getHost();
        String HostInIPv6;
        int pos1 = Host.indexOf("%");   // it indicates the address is IPv6.
        if(pos1 >= 0) {
            int pos2 = Host.indexOf("]");
            HostInIPv6 = Host.substring(0, pos1) + Host.substring(pos2, Host.length());
            Host = HostInIPv6;
            Log.d(TAG,"Host address of the resource(truncated): " + HostInIPv6);
        }

        String Uri = enrolleeResource.getUri();
        String devID = enrolleeResource.getServerId();
        Boolean isObservable = enrolleeResource.isObservable();
        EnumSet<OcConnectivityType> ConType = enrolleeResource.getConnectivityTypeSet();
        int conType = 0;
        for (OcConnectivityType type : ConType)
        {
            conType |= type.getValue();
        }
        List<String> resourceTypes = enrolleeResource.getResourceTypes();
        List<String> resourceInterfaces = enrolleeResource.getResourceInterfaces();

        if(!resourceTypes.contains(PROV_RESOURCE_TYPE)
                || !resourceInterfaces.contains(OcPlatform.BATCH_INTERFACE))
        {
            Log.e(TAG, "Validation check for OcResource is failed.");
            return null;
        }
        mRemoteEnrollee = nativeCreateRemoteEnrollee(Host, Uri, devID, isObservable, conType);
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
