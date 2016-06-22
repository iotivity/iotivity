/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

import android.util.Log;

public class RemoteEnrollee{

    public static final String TAG = RemoteEnrollee.class.getName();
    private long m_nativeHandle;

    private native void nativeSetCloudProvInfo(String authCode, String autoProvider, String ciServer);
    private native void nativeStartCloudProvisioning(CloudProvisioningCallback callback);

    /* constructor will be invoked from the native layer */
    private RemoteEnrollee(long nativeHandle){
        this.m_nativeHandle = nativeHandle;
    }

	/* native setCloudProvInfo */
    public void setCloudProvInfo(CloudProvInfo cloudInfo) throws ESException{
    	nativeSetCloudProvInfo(cloudInfo.getAuthCode(), cloudInfo.getAuthProvider(), cloudInfo.getCiServer());
	}

    /* native startCloudProvisioning */
    public void startCloudProvisioning(CloudProvisioningCallback callback) throws ESException{
        if(callback != null)
        {
            nativeStartCloudProvisioning(callback);
            return;
        }
        Log.d(TAG, "CloudProvisioningCallback is null ");
    }
}
